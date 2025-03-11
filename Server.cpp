#include "Server.hpp"
#include "Channel.hpp"
#include <sstream>

Server::Server(int port, std::string password) {
    this->port = port;
    this->password = password;
    this->sockfd = -1;
    this->max_fd = -1;
    
    initializeCommandHandlers();
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

Server::~Server() {
    for (size_t i = 0; i < connected_clients.size(); ++i) {
        close(connected_clients[i]);
    }
    close(sockfd);
}