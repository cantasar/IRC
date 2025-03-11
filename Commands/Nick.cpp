#include "../Server.hpp"

void Server::nick(size_t client_index, const std::string& command)
{
    std::vector<std::string> input = parseCommand(command);

    if (input.size() == 2)
    {
        if(isClientExist(input[1]) == false)
        {
            clients[client_index].setNickname(input[1]);
            clients[client_index].message(":" + clients[client_index].getNickname() + " NICK :" + clients[client_index].getNickname() + "\r\n");
        }
        else
        {
            clients[client_index].message(":" + clients[client_index].getNickname() + " 433 " + clients[client_index].getNickname() + " " + input[1] + " :Nickname is already in use\r\n");
        }
    }
    else
    {
        clients[client_index].message(":" + clients[client_index].getNickname() + " 461 " + clients[client_index].getNickname() + " NICK :Not enough parameters\r\n");
    }
}