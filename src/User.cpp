# include "User.hpp"

User::User() {
	this->_nickname = "";
	this->_username = "";
	this->_password = "";
}

User::User(int fd, std::string nickname, std::string username, std::string password) {
	this->_fd = fd;
	this->_nickname = nickname;
	this->_username = username;
	this->_password = password;
}

User::~User() {}

User::User(User const &src) {
	if (this != &src)
		*this = src;
}

User &User::operator=(User const &src) {
	if (this != &src) {
		this->_fd = src._fd;
		this->_nickname = src._nickname;
		this->_username = src._username;
		this->_password = src._password;
	}
	return *this;
}

int			User::getFd() const {
	return this->_fd;
}

std::string	User::getNickname() const {
	return this->_nickname;
}

std::string	User::getUsername() const {
	return this->_username;
}

std::string	User::getPassword() const {
	return this->_password;
}

void		User::setFd(int fd) {
	this->_fd = fd;
}

void		User::setNickname(std::string nickname) {
	this->_nickname = nickname;
}

void		User::setUsername(std::string username) {
	this->_username = username;
}

void		User::setPassword(std::string password) {
	this->_password = password;
}

