#include "Server.hpp"
#include "Channel.hpp"
#include <sstream>

Server::Server(int port, std::string password) : port(port), password(password)
{
    startServer();
    max_fd = sockfd;
    loopProgram();
}

Server::Server(Server const &server)
{
    port = server.port;
    password = server.password;
    sockfd = server.sockfd;
    clients = server.clients;
    connected_clients = server.connected_clients;
    read_fds = server.read_fds;
    max_fd = server.max_fd;
}

Server &Server::operator=(Server const &server)
{
    if (this != &server)
    {
        port = server.port;
        password = server.password;
        sockfd = server.sockfd;
        clients = server.clients;
        connected_clients = server.connected_clients;
        read_fds = server.read_fds;
        max_fd = server.max_fd;
    }
    return *this;
}

Server::~Server() {}

void Server::startServer()
{
    server_addr.sin_family = AF_INET;         // IPv4
    server_addr.sin_port = htons(port);       // Port
    server_addr.sin_addr.s_addr = INADDR_ANY; // Any IP

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket creation (SOCK_STREAM) and IPv4 (AF_INET) protocol (0)
    if (sockfd < 0)                           // If socket creation fails
    {
        close(sockfd);
        std::cout << "Socket KO" << std::endl;
        exit(1);
    }
    int opt = 1;
    // Attempt to set the socket option SO_REUSEADDR to allow the socket to be bound to an address that is already in use.
    // If setsockopt fails, close the socket, print an error message, and exit the program.
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(sockfd);
        std::cout << "Setsockopt KO" << std::endl;
        exit(1);
    }
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) // Bind the socket to the address and port specified in server_addr
    {
        close(sockfd);
        std::cout << "Bind KO" << std::endl;
        exit(1);
    }
    else
        std::cout << "Bind OK" << std::endl;
    if (listen(sockfd, 3) < 0) // Listen for incoming connections on the socket with a maximum queue length of 3
    {
        close(sockfd);
        std::cout << "Listen KO" << std::endl;
        exit(1);
    }
    else
        std::cout << "Listen OK" << std::endl;
    FD_ZERO(&read_fds);        // Clear the read_fds set
    FD_SET(sockfd, &read_fds); // Add the sockfd to the read_fds set
}

void Server::loopProgram()
{
    while (1)
    {
        fd_set active_fds = read_fds;
        int activity = select(max_fd + 1, &active_fds, NULL, NULL, NULL);
        if (activity < 0)
        {
            std::cout << "Select KO" << std::endl;
            exit(1);
        }
        if (FD_ISSET(sockfd, &active_fds)) // If the sockfd is in the active_fds set, a new client is trying to connect to the server and the server should accept the connection.
        {
            int client_sockfd = accept(sockfd, NULL, NULL);
            if (client_sockfd < 0)
            {
                close(client_sockfd);
                std::cout << "Accept KO" << std::endl;
                exit(1);
            }
            else
            {
                std::cout << "Accept OK" << std::endl;
                FD_SET(client_sockfd, &read_fds);
                connected_clients.push_back(client_sockfd);
                if (client_sockfd > max_fd)
                    max_fd = client_sockfd;
            }
            Client new_client(client_sockfd);
            this->clients.push_back(new_client);
        }

        int bytes_received;
        std::vector<char> buffer(1024, 0);
        for (size_t client_index = 0; client_index < connected_clients.size(); client_index++) // Loop through all connected clients to check if any of them have sent a message
        {
            if (FD_ISSET(connected_clients[client_index], &active_fds)) // If the client's socket is in the active_fds set, the client has sent a message to the server.
            {
                bytes_received = recv(connected_clients[client_index], buffer.data(), buffer.size(), 0); // Receive the message from the client

                printServer();

                checkCommands(buffer); // parse buffer here
                printAllInputs();

                executeCommand(client_index); // execute Command here
                printAllClients();

                logControl(client_index);
                
                for (size_t i = 0; i < channels.size(); ++i)
                {
                    channels[i].printChannel();
                }


                if (bytes_received < 0)
                {
                    close(connected_clients[client_index]);
                    std::cout << "Recv KO" << std::endl;
                    exit(1);
                }
                else if (bytes_received == 0)
                {
                    close(connected_clients[client_index]);
                    connected_clients.erase(connected_clients.begin() + client_index);
                    --client_index;
                    std::cout << "Client disconnected" << std::endl;
                    exit(1);
                }
                else
                {
                    logControl(client_index);
                    std::cout << "Received: " << buffer.data() << std::endl;
                    memset(buffer.data(), 0, buffer.size());
                }
            }
        }
    }
}

void Server::logControl(size_t client_index)
{

    if (clients[client_index].getPassword() != "" &&
        clients[client_index].getUsername() != "" &&
        clients[client_index].getNickname() != "" &&
        clients[client_index].getConnected() == false)
    {
        clients[client_index].setConnected(true);
        std::cout << "Client connected" << std::endl;
        clients[client_index].message(":" + clients[client_index].getIp_address() + " 001 " + clients[client_index].getNickname() + " :Welcome to the Internet Relay Network " + clients[client_index].getNickname() + "!" + clients[client_index].getUsername() + "@" + clients[client_index].getIp_address() + "\r\n");
    }
}

void Server::checkCommands(std::vector<char> &buffer)
{
    // Null karakterleri kaldırarak temiz bir string oluştur
    std::string buffer_string(buffer.data(), buffer.size());

    // Sondaki gereksiz '\r' ve '\n' karakterlerini temizle
    while (!buffer_string.empty() && (buffer_string.back() == '\r' || buffer_string.back() == '\n' || buffer_string.back() == '\0'))
    {
        buffer_string.pop_back();
    }

    std::istringstream iss(buffer_string);
    std::string token;
    
    bool realname_started = false;
    std::string realname;

    // Kelimeleri boşluklara göre ayır
    while (iss >> token) 
    {
        if (realname_started)
        {
            // `:` ile başlayan kısımdan sonra her şeyi tek bir string olarak al
            realname += (realname.empty() ? "" : " ") + token;
        }
        else if (token[0] == ':')
        {
            // `:` ile başlayan kısmı `realname` olarak belirle
            realname_started = true;
            realname = token.substr(1);
        }
        else
        {
            this->input.push_back(token);
        }
    }

    if (!realname.empty())
    {
        this->input.push_back(realname);
    }
}





void Server::executeCommand(size_t c_index)
{
    std::vector<std::string> commands;
    std::vector<fpoint> functions;

    commands.push_back("NICK");
    commands.push_back("USER");
    commands.push_back("PASS");
    commands.push_back("PRIVMSG");
    commands.push_back("JOIN");
    commands.push_back("TOPIC");
    commands.push_back("KICK");
    commands.push_back("MODE");
    commands.push_back("INVITE");
    commands.push_back("HELP");

    functions.push_back(&Server::nick);
    functions.push_back(&Server::user);
    functions.push_back(&Server::pass);
    functions.push_back(&Server::privmsg);
    functions.push_back(&Server::join);
    functions.push_back(&Server::topic);
    functions.push_back(&Server::kick);
    functions.push_back(&Server::mode);
    functions.push_back(&Server::invite);
    functions.push_back(&Server::help);


    for (size_t i = 0; i < commands.size(); i++)
    {
        for(size_t j = 0; j < this->input.size(); j++)
        {
            if (this->input[j] == commands[i])
            {
                (this->*functions[i])(c_index);
            }
        }
    }

    this->input.clear();
}
