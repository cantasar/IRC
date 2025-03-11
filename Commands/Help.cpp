#include "../Server.hpp"

void Server::help(size_t client_index, const std::string& command)
{
	(void)command;
	this->clients[client_index].message(":ircserv 704 " + this->clients[client_index].getNickname() + " :-----HELP MENU-----\r\n");
	
	if (this->clients[client_index].getConnected())
	{
		this->clients[client_index].message(":ircserv 704 " + this->clients[client_index].getNickname() + " : User logged in show useable commands\r\n");

	}
	else
	{
		this->clients[client_index].message(":ircserv 704 " + this->clients[client_index].getNickname() + " :You need login first\r\n");
		this->clients[client_index].message(":ircserv 704 " + this->clients[client_index].getNickname() + " :Use NICK <nickname>\r\n");
		this->clients[client_index].message(":ircserv 704 " + this->clients[client_index].getNickname() + " :Use USER <user> <mode> <unused> :<realname>\r\n");
		this->clients[client_index].message(":ircserv 704 " + this->clients[client_index].getNickname() + " :Use PASS <password>\r\n");
	}
}