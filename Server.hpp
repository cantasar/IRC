#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sstream>

class Channel;
class Client;

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

    std::vector<std::string> command_names;
    std::vector<void (Server::*)(size_t, const std::string&)> command_functions;

    void initializeCommandHandlers();
    void handleNewConnection();
    void handleClientData(size_t client_index, fd_set& active_fds);
    void processClientBuffer(size_t client_index);
    void disconnectClient(size_t client_index);

    std::vector<std::string> parseCommand(const std::string& command);

public:
    Server(int port, std::string password);
    Server(const Server& other);
    Server& operator=(const Server& other);
    ~Server();

    void startServer();
    void loopProgram();
    void logControl(size_t client_index);

    bool isClientExist(const std::string& nickName) const;

    void nick(size_t client_index, const std::string& command);
    void user(size_t client_index, const std::string& command);
    void pass(size_t client_index, const std::string& command);
    void privmsg(size_t client_index, const std::string& command);
    void join(size_t client_index, const std::string& command);
    void topic(size_t client_index, const std::string& command);
    void kick(size_t client_index, const std::string& command);
    void mode(size_t client_index, const std::string& command);
    void invite(size_t client_index, const std::string& command);
    void help(size_t client_index, const std::string& command);

    void handleCommand(size_t client_index, const std::string &command);

    // Test methods
    void printAllInputs(const std::string &command);
    void printAllClients();
    void printServer();

};

#endif