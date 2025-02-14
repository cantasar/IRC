#include "../Server.hpp"

void Server::topic(size_t client_index)
{
    // Check if client is connected
    if (!clients[client_index].getConnected())
    {
        clients[client_index].message(":server 451 " + clients[client_index].getNickname() + " :You have not registered\r\n");
        return;
    }

    // Check if we have enough parameters (TOPIC <channel> <topic>)
    if (input.size() < 3)
    {
        clients[client_index].message(":server 461 " + clients[client_index].getNickname() + " TOPIC :Not enough parameters\r\n");
        clients[client_index].message(":server 461 " + clients[client_index].getNickname() + " TOPIC :Useage TOPIC <channel> <topic>\r\n");
        return;
    }

    std::string channel_name = input[1];
    std::string topic = input[2];

    // Check if channel name starts with '#'
    if (channel_name[0] != '#')
    {
        clients[client_index].message(":server 476 " + clients[client_index].getNickname() + " " + channel_name + " :Invalid channel name\r\n");
        return;
    }

    // Check if channel already exists
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        if (it->getName() == channel_name)
        {
            // Check if client is not in the channel
            if (it->isClientInChannel(clients[client_index].getNickname()) == false)
            {
                clients[client_index].message(":server 442 " + clients[client_index].getNickname() + " " + channel_name + " :You're not on that channel\r\n");
                return;
            }

            // Check if topic protection is enabled and user is not operator
            if (it->getTopicProtection() && !it->isOperator(clients[client_index]))
            {
                clients[client_index].message(":server 482 " + clients[client_index].getNickname() + " " + channel_name + " :You're not channel operator\r\n");
                return;
            }

            // Set topic
            it->setTopic(topic);
            std::string topic_message = ":" + clients[client_index].getNickname() + "!" + 
                                        clients[client_index].getUsername() + "@" + 
                                        clients[client_index].getIp_address() + 
                                        " TOPIC " + channel_name + " :" + topic + "\r\n";
            std::vector<Client> channel_clients = it->getClients();

            // Send topic message to all clients in the channel
            for (std::vector<Client>::iterator client_it = channel_clients.begin(); client_it != channel_clients.end(); ++client_it)
            {
                client_it->message(topic_message);
            }
            return;
        }
    }

    // Channel not found
    clients[client_index].message(":server 403 " + clients[client_index].getNickname() + " " + channel_name + " :No such channel\r\n");
}