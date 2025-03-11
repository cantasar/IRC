#include "../Server.hpp"
#include "../Channel.hpp"

void Server::privmsg(size_t client_index, const std::string& command)
{
    std::vector<std::string> input = parseCommand(command);

    // Check if client is connected
    if (!clients[client_index].getConnected())
    {
        clients[client_index].message(":server 451 " + clients[client_index].getNickname() + " :You have not registered\r\n");
        return;
    }

    // Check if we have enough parameters (PRIVMSG <target> <message>)
    if (input.size() < 3)
    {
        clients[client_index].message(":server 461 " + clients[client_index].getNickname() + " PRIVMSG :Not enough parameters\r\n");
        clients[client_index].message(":server 461 " + clients[client_index].getNickname() + " PRIVMSG :Useage PRIVMSG <target> <message>\r\n");
        return;
    }

    std::string target = input[1];
    std::string message = input[2];

    // If target starts with #, it's a channel message
    if (target[0] == '#')
    {
        bool channel_found = false;
        for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
        {
            if (it->getName() == target)
            {
                channel_found = true;
                // Check if client is in the channel
                if (!it->isClientInChannel(clients[client_index].getNickname()))
                {
                    clients[client_index].message(":server 442 " + clients[client_index].getNickname() + " " + target + " :You're not on that channel\r\n");
                    return;
                }
                // Send message to all clients in the channel except the sender
                std::vector<Client> channel_clients = it->getClients();
                for (std::vector<Client>::iterator cit = channel_clients.begin(); cit != channel_clients.end(); ++cit)
                {
                    if (cit->getNickname() != clients[client_index].getNickname())
                    {
                        cit->message(":" + clients[client_index].getNickname() + "!" + 
                                   clients[client_index].getUsername() + "@" + 
                                   clients[client_index].getIp_address() + 
                                   " PRIVMSG " + target + " :" + message + "\r\n");
                    }
                }
                break;
            }
        }
        if (!channel_found)
        {
            clients[client_index].message(":server 403 " + clients[client_index].getNickname() + " " + target + " :No such channel\r\n");
        }
    }
    // Otherwise, it's a private message to a user
    else
    {
        bool user_found = false;
        for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
        {
            if (it->getNickname() == target)
            {
                user_found = true;
                it->message(":" + clients[client_index].getNickname() + "!" + 
                           clients[client_index].getUsername() + "@" + 
                           clients[client_index].getIp_address() + 
                           " PRIVMSG " + target + " :" + message + "\r\n");
                break;
            }
        }
        if (!user_found)
        {
            clients[client_index].message(":server 401 " + clients[client_index].getNickname() + " " + target + " :No such nick/channel\r\n");
        }
    }
}

