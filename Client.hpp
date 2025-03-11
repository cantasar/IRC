#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
#include <string>

class Client
{
private:
    int c_sockfd;
    bool connected;
    std::string username;
    std::string nickname;
    std::string password;
    std::string ip_address;
    // KOMUT BUFFERI
    std::string command_buffer;

public:
    Client(int fd);
    Client(Client const &client);
    Client &operator=(Client const &client);
    ~Client();

    int getC_sockfd() const;
    void setC_sockfd(int c_sockfd);

    bool getConnected() const;
    void setConnected(bool connected);

    std::string getUsername() const;
    void setUsername(std::string username);

    std::string getNickname() const;
    void setNickname(std::string nickname);

    std::string getPassword() const;
    void setPassword(std::string password);

    std::string getIp_address() const;
    void setIp_address(const std::string &ip_address);

    void message(const std::string &message) const;

    std::string &getCommandBuffer();
    void appendToCommandBuffer(const std::string &data);
    void clearCommandBuffer();
};

#endif