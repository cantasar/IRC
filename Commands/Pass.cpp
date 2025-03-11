#include "../Server.hpp"

void Server::pass(size_t client_index, const std::string& command)
{
    std::vector<std::string> input = parseCommand(command);

    if (input.size() == 2)
    {
        clients[client_index].setPassword(input[1]);
        clients[client_index].message(":" + clients[client_index].getNickname() + " PASS :" + clients[client_index].getPassword() + "\r\n");
    }
    else
    {
        clients[client_index].message(":" + clients[client_index].getNickname() + " 461 " + clients[client_index].getNickname() + " PASS :Not enough parameters\r\n");
    }
}
