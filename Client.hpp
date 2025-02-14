#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <vector>
#include <sys/socket.h>

class Client{
    private:
        int c_sockfd;
        bool connected;
        std::string username;
        std::string nickname;
        std::string password;
        std::string ip_address;
    public:
        Client(int fd);
        Client(Client const &client);
        Client &operator=(Client const &client);
        ~Client();

        int getC_sockfd();
        void setC_sockfd(int c_sockfd);

        bool getConnected();
        void setConnected(bool connected);

        std::string getUsername();
        void setUsername(std::string username);

        std::string getNickname() const;
        void setNickname(std::string nickname);

        std::string getPassword();
        void setPassword(std::string password);

        std::string getIp_address();
        void setIp_address(int ip_address);

        void message(std::string message);
};

#endif