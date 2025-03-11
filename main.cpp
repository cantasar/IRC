#include "Server.hpp"
#include <limits>
#include <stdexcept>

void signalHandler(int code)
{
	std::cout << "Signal " << code << " caught!" << std::endl;
	exit(0);
}

bool isValidPort(const std::string& portStr)
{
	if (portStr.empty())
		return false;
		
	for (char c : portStr)
	{
		if (!std::isdigit(c))
			return false;
	}
	
	try
	{
		long port = std::stol(portStr);
		return port >= 0 && port <= 65535;
	}
	catch(const std::exception&)
	{
		return false;
	}
}

int main(int ac, char **av)
{
	signal(SIGINT, signalHandler);

	if (ac != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	if (!av[1] || !av[2])
	{
		std::cerr << "Error: Invalid arguments" << std::endl;
		return 1;
	}

	if (!isValidPort(av[1]))
	{
		std::cerr << "Error: Port must be a number between 0 and 65535" << std::endl;
		return 1;
	}

	try
	{
		Server server(std::stoi(av[1]), av[2]);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}