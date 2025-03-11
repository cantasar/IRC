#include "../Server.hpp"
#include "../Channel.hpp"

void Server::printAllInputs(const std::string &command)
{
	std::cout << "\n=== Command Input Debug ===" << std::endl;
	std::cout << "Raw command: " << command << std::endl;
	std::cout << "Parsed components:" << std::endl;
	
	std::vector<std::string> input = parseCommand(command);
	int index = 0;
	for (std::vector<std::string>::iterator it = input.begin(); it != input.end(); it++, index++)
	{
		std::cout << "[" << index << "]: " << *it << std::endl;
	}
	std::cout << "Total components: " << input.size() << std::endl;
	std::cout << "=========================" << std::endl;
}

void Server::printAllClients()
{
	std::cout << "\n=== Connected Clients Debug ===" << std::endl;
	std::cout << "Total clients: " << this->clients.size() << std::endl;
	
	int index = 0;
	for (std::vector<Client>::iterator it = this->clients.begin(); it != this->clients.end(); it++, index++)
	{
		std::cout << "\nClient #" << index + 1 << ":" << std::endl;
		std::cout << "├─ Socket FD: " << it->getC_sockfd() << std::endl;
		std::cout << "├─ Nickname: " << it->getNickname() << std::endl;
		std::cout << "├─ Username: " << it->getUsername() << std::endl;
		std::cout << "├─ Password: " << (it->getPassword().empty() ? "[Not set]" : it->getPassword()) << std::endl;
		std::cout << "├─ IP Address: " << it->getIp_address() << std::endl;
		std::cout << "└─ Connection Status: " << (it->getConnected() ? "Connected" : "Disconnected") << std::endl;
	}
	std::cout << "===========================" << std::endl;
}

void Server::printServer()
{
	std::cout << "\n=== Server Status Debug ===" << std::endl;
	std::cout << "├─ Socket FD: " << this->sockfd << std::endl;
	std::cout << "├─ Port: " << this->port << std::endl;
	std::cout << "├─ Password: " << (this->password.empty() ? "[Not set]" : this->password) << std::endl;
	std::cout << "├─ Max FD: " << this->max_fd << std::endl;
	std::cout << "├─ Total Clients: " << this->clients.size() << std::endl;
	std::cout << "└─ Total Channels: " << this->channels.size() << std::endl;
	std::cout << "========================" << std::endl;
}

void Channel::printChannel()
{
	std::cout << "\n=== Channel Debug ===" << std::endl;
	std::cout << "├─ Name: " << this->_name << std::endl;
	std::cout << "├─ Topic: " << (this->_topic.empty() ? "[No topic set]" : this->_topic) << std::endl;
	std::cout << "├─ Key Status: " << (this->getChannelKey() ? "Protected" : "No key") << std::endl;
	std::cout << "├─ Key: " << (this->_key.empty() ? "[Not set]" : this->_key) << std::endl;
	
	std::cout << "├─ Clients (" << this->_clients.size() << "):" << std::endl;
	if (this->_clients.empty())
		std::cout << "│  └─ [No clients]" << std::endl;
	else
		for (std::vector<Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
			std::cout << "│  ├─ " << it->getNickname() << " (" << it->getUsername() << ")" << std::endl;

	std::cout << "├─ Invited Clients (" << this->_invitedClients.size() << "):" << std::endl;
	if (this->_invitedClients.empty())
		std::cout << "│  └─ [No invited clients]" << std::endl;
	else
		for (std::vector<Client>::iterator it = this->_invitedClients.begin(); it != this->_invitedClients.end(); it++)
			std::cout << "│  ├─ " << it->getNickname() << " (" << it->getUsername() << ")" << std::endl;
	
	std::cout << "└─ Operators (" << this->_operators.size() << "):" << std::endl;
	if (this->_operators.empty())
		std::cout << "   └─ [No operators]" << std::endl;
	else
		for (std::vector<Client>::iterator it = this->_operators.begin(); it != this->_operators.end(); it++)
			std::cout << "   ├─ " << it->getNickname() << " (" << it->getUsername() << ")" << std::endl;
	
	std::cout << "====================" << std::endl;
}
