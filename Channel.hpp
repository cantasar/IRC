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
        std::vector<Client> _invitedClients;

        bool inviteOnly; // +i
        bool topicProtection; // +t
        bool channelKey; // +k
        bool channelLimit; // +l

        std::string _key;
        int _limit;

    public:
        Channel(std::string name, std::string topic);
        Channel(Channel const &channel);
        Channel &operator=(Channel const &channel);
        ~Channel();

        void setName(std::string name);
        std::string getName() const;

        void setTopic(std::string topic);
        std::string getTopic() const;

        void addClient(Client client);
        void removeClient(Client client);
        std::vector<Client> getClients() const;

        void addOperator(Client client);
        void removeOperator(Client client);
        bool isOperator(Client client) const;
        std::vector<Client> getOperators() const;

        bool isClientInChannel(std::string nickname) const;

        void setInviteOnly(bool inviteOnly);
        bool getInviteOnly() const;

        void setTopicProtection(bool topicProtection);
        bool getTopicProtection() const;

        void setChannelKey(bool channelKey);
        bool getChannelKey() const;

        void setChannelLimit(bool channelLimit);
        bool getChannelLimit() const;

        void setKey(std::string key);
        std::string getKey() const;

        void setLimit(int limit);
        int getLimit() const;

        //development tests
        void printChannel();

        void addInvited(Client client);
        void removeInvited(Client client);
        bool isInvited(Client client) const;
};

#endif