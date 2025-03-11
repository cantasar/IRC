#include "Client.hpp"

Client::Client(int fd)
{
    c_sockfd = fd;
    connected = false;
    username = "";
    nickname = "";
    password = "";
    ip_address = "";
}

Client::Client(Client const &client)
{
    c_sockfd = client.c_sockfd;
    connected = client.connected;
    username = client.username;
    nickname = client.nickname;
    password = client.password;
    ip_address = client.ip_address;
}

Client &Client::operator=(Client const &client)
{
    if (this != &client)
    {
        c_sockfd = client.c_sockfd;
        connected = client.connected;
        username = client.username;
        nickname = client.nickname;
        password = client.password;
        ip_address = client.ip_address;
    }
    return *this;
}

Client::~Client() {
}

int Client::getC_sockfd() const
{
    return c_sockfd;
}
void Client::setC_sockfd(int c_sockfd)
{
    this->c_sockfd = c_sockfd;
}

bool Client::getConnected() const
{
    return connected;
}

void Client::setConnected(bool connected)
{
    this->connected = connected;
}

std::string Client::getUsername() const
{
    return username;
}

void Client::setUsername(std::string username)
{
    this->username = username;
}

std::string Client::getNickname() const
{
    return nickname;
}

void Client::setNickname(std::string nickname)
{
    this->nickname = nickname;
}

std::string Client::getPassword() const
{
    return password;
}

void Client::setPassword(std::string password)
{
    this->password = password;
}

std::string Client::getIp_address() const
{
    return ip_address;
}

void Client::setIp_address(const std::string &ip_address)
{
    this->ip_address = ip_address;
}

void Client::message(const std::string &message) const
{
    send(c_sockfd, message.c_str(), message.size(), 0);
}

std::string &Client::getCommandBuffer()
{
    return command_buffer;
}
void Client::appendToCommandBuffer(const std::string &data)
{
    command_buffer += data;
}
void Client::clearCommandBuffer()
{
    command_buffer.clear();
}