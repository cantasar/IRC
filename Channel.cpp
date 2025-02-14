#include "Channel.hpp"

Channel::Channel(std::string name, std::string topic): _name(name), _topic(topic){}

Channel::Channel(Channel const &channel): _name(channel._name), _topic(channel._topic), _clients(channel._clients), _operators(channel._operators){}

Channel &Channel::operator=(Channel const &channel){
    if (this != &channel){
        _name = channel._name;
        _topic = channel._topic;
        _clients = channel._clients;
        _operators = channel._operators;
    }
    return *this;
}

Channel::~Channel(){}

void Channel::setName(std::string name){
    _name = name;
}
std::string Channel::getName() const{
    return _name;
}

void Channel::setTopic(std::string topic){
    _topic = topic;
}

std::string Channel::getTopic() const{
    return _topic;
}

void Channel::addClient(Client client){
    _clients.push_back(client);
}

void Channel::removeClient(Client client){
    for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it){
        if (it->getNickname() == client.getNickname()){
            _clients.erase(it);
            break;
        }
    }
}

std::vector<Client> Channel::getClients() const{
    return _clients;
}

void Channel::addOperator(Client client){
    _operators.push_back(client);
}
void Channel::removeOperator(Client client){
    for (std::vector<Client>::iterator it = _operators.begin(); it != _operators.end(); ++it){
        if (it->getNickname() == client.getNickname()){
            _operators.erase(it);
            break;
        }
    }
}

bool Channel::isOperator(Client client) const{
    for (std::vector<Client>::const_iterator it = _operators.begin(); it != _operators.end(); ++it){
        if (it->getNickname() == client.getNickname()){
            return true;
        }
    }
    return false;
}
std::vector<Client> Channel::getOperators() const{
    return _operators;
}

bool Channel::isClientInChannel(std::string nickname) const{
    for (std::vector<Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it){
        if (it->getNickname() == nickname){
            return true;
        }
    }
    return false;
}

void Channel::setInviteOnly(bool inviteOnly){
    this->inviteOnly = inviteOnly;
}

bool Channel::getInviteOnly() const{
    return inviteOnly;
}

void Channel::setTopicProtection(bool topicProtection){
    this->topicProtection = topicProtection;
}

bool Channel::getTopicProtection() const{
    return topicProtection;
}

void Channel::setChannelKey(bool channelKey){
    this->channelKey = channelKey;
}

bool Channel::getChannelKey() const{
    return channelKey;
}

void Channel::setChannelLimit(bool channelLimit){
    this->channelLimit = channelLimit;
}

bool Channel::getChannelLimit() const{
    return channelLimit;
}

void Channel::setKey(std::string key){
    _key = key;
}

std::string Channel::getKey() const{
    return _key;
}

void Channel::setLimit(int limit){
    _limit = limit;
}

int Channel::getLimit() const{
    return _limit;
}

void Channel::addInvited(Client client) {
    _invitedClients.push_back(client);
}

void Channel::removeInvited(Client client) {
    for (std::vector<Client>::iterator it = _invitedClients.begin(); it != _invitedClients.end(); ++it) {
        if (it->getNickname() == client.getNickname()) {
            _invitedClients.erase(it);
            break;
        }
    }
}

bool Channel::isInvited(Client client) const {
    for (std::vector<Client>::const_iterator it = _invitedClients.begin(); it != _invitedClients.end(); ++it) {
        if (it->getNickname() == client.getNickname()) {
            return true;
        }
    }
    return false;
}
