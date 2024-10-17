# include "Client.hpp"

Client::Client(): _isRegistered(false) {}

Client::~Client() {}

Client::Client(Client const &src) {
	if (this != &src)
		*this = src;
}

Client &Client::operator=(Client const &src) {
	if (this != &src) {
		this->_fd = src._fd;
		this->_ip = src._ip;
		this->_user = src._user;
		this->_isRegistered = src._isRegistered;
	}
	return *this;
}

int			Client::getFd() {
	return this->_fd;
}

std::string	Client::getIp() {
	return this->_ip;
}

User		Client::getUser() {
	return this->_user;
}

bool		Client::getIsRegistered() {
	return this->_isRegistered;
}

void		Client::setFd(int fd) {
	this->_fd = fd;
}

void		Client::setIp(std::string ip) {
	this->_ip = ip;
}

void		Client::setUser(User *user) {
	this->_user = *user;
}

void		Client::setIsRegistered(bool isRegistered) {
	this->_isRegistered = isRegistered;
}