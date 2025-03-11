#include "Server.hpp"

void Server::initializeCommandHandlers()
{
    command_names.push_back("NICK");
    command_names.push_back("USER");
    command_names.push_back("PASS");
    command_names.push_back("PRIVMSG");
    command_names.push_back("JOIN");
    command_names.push_back("TOPIC");
    command_names.push_back("KICK");
    command_names.push_back("MODE");
    command_names.push_back("INVITE");
    command_names.push_back("HELP");

    command_functions.push_back(&Server::nick);
    command_functions.push_back(&Server::user);
    command_functions.push_back(&Server::pass);
    command_functions.push_back(&Server::privmsg);
    command_functions.push_back(&Server::join);
    command_functions.push_back(&Server::topic);
    command_functions.push_back(&Server::kick);
    command_functions.push_back(&Server::mode);
    command_functions.push_back(&Server::invite);
    command_functions.push_back(&Server::help);
}

void Server::processClientBuffer(size_t client_index)
{
    std::string &client_buffer = clients[client_index].getCommandBuffer();
    std::string::size_type pos;

    while ((pos = client_buffer.find("\n")) != std::string::npos)
    {
        std::string command = client_buffer.substr(0, pos);
        if (!command.empty() && command[command.length() - 1] == '\r')
            command = command.substr(0, command.length() - 1);

        handleCommand(client_index, command);
        client_buffer.erase(0, pos + 1);
    }
}

std::vector<std::string> Server::parseCommand(const std::string &command)
{
    std::vector<std::string> input;

    // Return empty vector for empty command
    if (command.empty())
    {
        return input;
    }

    std::istringstream iss(command);
    std::string token;

    // Get the command name
    if (iss >> token)
    {
        input.push_back(token);

        // Read remaining parameters
        while (iss >> token)
        {
            if (!token.empty() && token[0] == ':')
            {
                // If parameter starts with ':', read the rest of the line
                std::string rest;
                std::getline(iss, rest);

                // If there's content after ':', append it
                if (token.length() > 1)
                {
                    rest = token.substr(1) + rest;
                }

                // Only add non-empty trailing parameter
                if (!rest.empty())
                {
                    input.push_back(rest);
                }
                break;
            }
            else
            {
                input.push_back(token);
            }
        }
    }

    return input;
}

void Server::handleCommand(size_t client_index, const std::string &command)
{

    if (command.empty())
    {
        return;
    }

    std::vector<std::string> input = parseCommand(command);
    if (input.empty())
    {
        return;
    }

    std::string cmd_name = input[0];

    for (size_t i = 0; i < command_names.size(); ++i)
    {
        if (command_names[i] == cmd_name)
        {
            (this->*command_functions[i])(client_index, command);
            break;
        }
    }

    if (!clients[client_index].getConnected())
    {
        logControl(client_index);
    }
}

void Server::logControl(size_t client_index)
{
    if (clients[client_index].getPassword() != "" &&
        clients[client_index].getUsername() != "" &&
        clients[client_index].getNickname() != "" &&
        clients[client_index].getConnected() == false)
    {
        clients[client_index].setConnected(true);
        std::cout << "Client connected" << std::endl;
        clients[client_index].message(":" + clients[client_index].getIp_address() + " 001 " + clients[client_index].getNickname() + " :Welcome to the Internet Relay Network " + clients[client_index].getNickname() + "!" + clients[client_index].getUsername() + "@" + clients[client_index].getIp_address() + "\r\n");
    }
}

bool Server::isClientExist(const std::string &nickName) const
{
    for (size_t i = 0; i < clients.size(); ++i)
    {
        if (nickName == clients[i].getNickname())
            return true;
    }
    return false;
}