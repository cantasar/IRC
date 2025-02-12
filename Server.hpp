#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <cstring>


class Server
{
private:
    int sockfd;
    int port;
    std::string password;

    std::vector<Client> clients; 
    std::vector<Channel> channels; 
    std::vector<int> connected_clients;
    
    fd_set read_fds;
    struct sockaddr_in server_addr;

    int max_fd;

    std::vector<std::string> input;
    typedef void(Server::*fpoint)(size_t);

public:
    Server(int port, std::string password);
    Server(Server const &server);
    Server &operator=(Server const &server);
    ~Server();

    void startServer();
    void loopProgram();
    void logControl(size_t client_index);

    void checkCommands(std::vector<char> &buffer);
    void executeCommand(size_t client_index);

    bool isClientExist(std::string nickName);

    void nick(size_t client_index);
    void user(size_t client_index);
    void pass(size_t client_index);
    void privmsg(size_t client_index);
    void join(size_t client_index);
    void topic(size_t client_index);
    void help(size_t client_index);

    // test
    void printAllClients();
    void printAllInputs();
    void printServer();
};

#endif