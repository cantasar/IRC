#include "../Server.hpp"
#include "../Channel.hpp"

bool Channel::isClientInChannel(std::string nickname)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getNickname() == nickname)
		{
			return true;
		}
	}
	return false;
}

void Server::join(size_t client_index)
{
	// Check if client is connected
	if (!clients[client_index].getConnected())
	{
		clients[client_index].message(":server 451 " + clients[client_index].getNickname() + " :You have not registered\r\n");
		return;
	}

	// Check if we have enough parameters (JOIN <channel>)
	if (input.size() < 2)
	{
		clients[client_index].message(":server 461 " + clients[client_index].getNickname() + " JOIN :Not enough parameters\r\n");
		clients[client_index].message(":server 461 " + clients[client_index].getNickname() + " JOIN :Useage JOIN <channel>\r\n");
		return;
	}

	std::string channel_name = input[1];

	// Check if channel already exists
	for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		if (it->getName() == channel_name)
		{
			// Check if client is already in the channel
			if (it->isClientInChannel(clients[client_index].getNickname()) == true)
			{
				clients[client_index].message(":server 405 " + clients[client_index].getNickname() + " " + channel_name + " :You are already in that channel\r\n");
				return;
			}

			// Add client to the channel
			it->addClient(clients[client_index]);
			clients[client_index].message(":" + clients[client_index].getNickname() + "!" + 
										clients[client_index].getUsername() + "@" + 
										clients[client_index].getIp_address() + 
										" JOIN " + channel_name + "\r\n");
			return;
		}
	}

	// If channel does not exist, create it
	Channel new_channel(channel_name, "");
	new_channel.addClient(clients[client_index]);
	channels.push_back(new_channel);
	clients[client_index].message(":" + clients[client_index].getNickname() + "!" + 
								clients[client_index].getUsername() + "@" + 
								clients[client_index].getIp_address() + 
								" JOIN " + channel_name + "\r\n");
}
