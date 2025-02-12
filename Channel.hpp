#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>
#include "Client.hpp"

class Channel{
    private:
        std::string _name;
        std::string _topic;
        std::vector<Client> _clients;
        std::vector<Client> _operators;

    public:
        Channel(std::string name, std::string topic);
        Channel(Channel const &channel);
        Channel &operator=(Channel const &channel);
        ~Channel();

        void setName(std::string name);
        std::string getName();

        void setTopic(std::string topic);
        std::string getTopic();

        void addClient(Client client);
        void removeClient(Client client);
        std::vector<Client> getClients();

        void addOperator(Client client);
        void removeOperator(Client client);
        bool isOperator(Client client);
        std::vector<Client> getOperators();

        bool isClientInChannel(std::string nickname);



        void printChannel();


};

#endif