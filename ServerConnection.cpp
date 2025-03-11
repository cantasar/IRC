#include "Server.hpp"

void Server::startServer()
{
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        close(sockfd);
        std::cerr << "Socket KO" << std::endl;
        exit(1);
    }
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(sockfd);
        std::cerr << "Setsockopt SO_REUSEADDR KO" << std::endl;
        exit(1);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    {
        close(sockfd);
        std::cerr << "Setsockopt SO_REUSEPORT KO" << std::endl;
        exit(1);
    }
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        close(sockfd);
        std::cerr << "Bind KO" << std::endl;
        exit(1);
    }
    else
        std::cout << "Bind OK" << std::endl;
    if (listen(sockfd, 3) < 0)
    {
        close(sockfd);
        std::cerr << "Listen KO" << std::endl;
        exit(1);
    }
    else
        std::cout << "Listen OK" << std::endl;
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);
}

void Server::loopProgram()
{
    while (true)
    {
        fd_set active_fds = read_fds;
        select(max_fd + 1, &active_fds, NULL, NULL, NULL);
        if (FD_ISSET(sockfd, &active_fds))
        {
            handleNewConnection();
        }

        for (size_t client_index = 0; client_index < connected_clients.size();)
        {
            if (FD_ISSET(connected_clients[client_index], &active_fds))
            {
                handleClientData(client_index, active_fds);
                client_index++;
            }
            else
            {
                client_index++;
            }
        }
    }
}

void Server::handleNewConnection() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (client_sockfd < 0) {
        std::cerr << "Accept failed" << std::endl;
        return;
    }

    // Get client IP address
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, INET_ADDRSTRLEN);

    FD_SET(client_sockfd, &read_fds);
    connected_clients.push_back(client_sockfd);
    max_fd = client_sockfd > max_fd ? client_sockfd : max_fd;
    
    Client new_client(client_sockfd);
    new_client.setIp_address(ip_str);
    clients.push_back(new_client);
    std::cout << "New client connected from " << ip_str << std::endl;
}

void Server::handleClientData(size_t client_index, fd_set& active_fds) {
    if (!FD_ISSET(connected_clients[client_index], &active_fds))
        return;

    std::vector<char> buffer(1024, 0);

    int bytes_received = recv(connected_clients[client_index], &buffer[0], buffer.size(), 0);

    if (bytes_received <= 0) {
        disconnectClient(client_index);
        return;
    }

    printServer(); // Test method call - to be removed later
    printAllClients(); // Test method call - to be removed later

    for (size_t i = 0; i < channels.size(); ++i)
    {
        channels[i].printChannel();
    }

    std::string received_data(&buffer[0], bytes_received);
    clients[client_index].appendToCommandBuffer(received_data);
    
    processClientBuffer(client_index);
}

void Server::disconnectClient(size_t client_index) {
    int client_fd = connected_clients[client_index];
    close(client_fd);
    FD_CLR(client_fd, &read_fds);
    connected_clients.erase(connected_clients.begin() + client_index);
    clients.erase(clients.begin() + client_index);
    std::cout << "Client disconnected" << std::endl;
} 