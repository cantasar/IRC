#include "../Server.hpp"
#include "../Channel.hpp"

void Server::printAllInputs()
{
	std::cout << "Printing all inputs:" << std::endl;
	for (size_t i = 0; i < this->input.size(); i++)
	{
		std::cout << "input[" << i << "]: " << this->input[i] << "~" << std::endl;
	}
	std::cout << "Total number of inputs: " << this->input.size() << std::endl;
	std::cout << std::endl;
}

void Server::printAllClients()
{
	std::cout << std::endl;
	for (std::vector<Client>::iterator it = this->clients.begin(); it != this->clients.end(); it++)
	{
		std::cout << "Client sockfd: " << it->getC_sockfd() << std::endl;
		std::cout << "Client Nickname: " << it->getNickname() << std::endl;
		std::cout << "Client Username: " << it->getUsername() << std::endl;
		std::cout << "Client Password: " << it->getPassword() << std::endl;
		std::cout << "Client IP Address: " << it->getIp_address() << std::endl;
		std::cout << "Client Connected: " << it->getConnected() << std::endl;
		std::cout << std::endl;
	}
}

void Server::printServer()
{
	std::cout << "Server sockfd: " << this->sockfd << std::endl;
	std::cout << "Server Port: " << this->port << std::endl;
	std::cout << "Server Password: " << this->password << std::endl;
	std::cout << "Server Max fd: " << this->max_fd << std::endl;
	std::cout << std::endl;
}

void Channel::printChannel()
{
	std::cout << "Channel Name: " << this->_name << std::endl;
	std::cout << "Channel Topic: " << this->_topic << std::endl;
	std::cout << "Channel Clients: " << std::endl;
	for (std::vector<Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		std::cout << "-" << it->getNickname() << std::endl;
	}
	std::cout << "Channel Operators: " << std::endl;
	for (std::vector<Client>::iterator it = this->_operators.begin(); it != this->_operators.end(); it++)
	{
		std::cout << "-" << it->getNickname() << std::endl;
	}
	std::cout << std::endl;
}
