# include "Server.hpp"

bool	Server::_signal = false;
Server::Server(int port, std::string password) {
	_port = port;
	_password = password;
}

Server::~Server() {}

Server::Server(Server const &src) {
	if (this != &src)
		*this = src;
}

Server &Server::operator=(Server const &src) {
	if (this != &src) {
		this->_port = src._port;
		this->_password = src._password;
	}
	return *this;
}

int	Server::getPort() {
	return this->_port;
}

std::string	Server::getPassword() {
	return this->_password;
}

User	*Server::getUser(int fd) {
	for (size_t i = 0; i < _users.size(); i++) {
		if (_users[i].getFd() == fd)
			return &_users[i];
	}
	return NULL;
}

void	Server::init() {
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	createSocket();

	while (Server::_signal == false)
	{
		if((poll(&_fds[0], _fds.size(), -1) == -1) && Server::_signal == false)
			throw(std::runtime_error("poll() faild"));

		for (size_t i = 0; i < _fds.size(); i++)
		{
			if (_fds[i].revents & POLLIN)
			{
				if (_fds[i].fd == _fd)
					acceptNewClient();
				else
					receiveData(_fds[i].fd);
			}
		}
	}
	closeFds();
}

void	Server::createSocket() {
	struct sockaddr_in add;
	struct pollfd newPoll;

	add.sin_family = AF_INET;
	add.sin_port = htons(this->_port);
	add.sin_addr.s_addr = INADDR_ANY;

	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(_fd == -1)
		throw(std::runtime_error("faild to create socket"));

	int en = 1;
	if(setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
	if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
	if (bind(_fd, (struct sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("faild to bind socket"));
	if (listen(_fd, SOMAXCONN) == -1)
		throw(std::runtime_error("listen() faild"));

	newPoll.fd = _fd;
	newPoll.events = POLLIN;
	newPoll.revents = 0;
	_fds.push_back(newPoll);

	MSG(GREEN, "Server", "Connected !");
	std::cout << "Waiting for clients..." << std::endl;
}

void	Server::sendMessage(int fd, std::string msg) {
	send(fd, msg.c_str(), msg.size(), 0);
}

void	Server::acceptNewClient() {
	Client client;
	struct sockaddr_in newClient;
	struct pollfd newPoll;
	socklen_t len = sizeof(newClient);

	int newClientFd = accept(_fd, (sockaddr *)&(newClient), &len);
	if (newClientFd == -1) {
		ERR("accept() failed");
		return;
	}

	if (fcntl(newClientFd, F_SETFL, O_NONBLOCK) == -1) {
		ERR("fcntl() failed");
		return;
	}

	newPoll.fd = newClientFd;
	newPoll.events = POLLIN;
	newPoll.revents = 0;

	client.setFd(newClientFd);
	client.setIp(inet_ntoa((newClient.sin_addr)));
	_clients.push_back(client);
	_fds.push_back(newPoll);

	User newUser(newClientFd, "", "", "");
	_users.push_back(newUser);

	client.setUser(&newUser);

	CLIENT_MSG(GREEN, "Client", "Client ", newClientFd, " is connected !");
	sendMessage(newClientFd, "Welcome to the server !\n");
}

void	Server::receiveData(int fd) {
	char buff[1024];
	memset(buff, 0, sizeof(buff));

	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1 , 0);

	if (bytes <= 0) {
		CLIENT_MSG(RED, "Client", "Client ", fd, " has been disconnected.");
		clearClient(fd);
		close(fd);
	} else {
		buff[bytes] = '\0';
		CLIENT_MSG(BLUE, "Client", "Data received: ", fd, buff);
		//here you can add your code to process the received data: parse, check, authenticate, handle the command, etc...
		// std::cout << "\n-------------------\nraw data:\n" << buff << "\n------------------------\n" << std::endl;
		
		User *user = getUser(fd);
		if (!user)
			return ;
		
		parseData(fd, buff);
	}
}

void	Server::signalHandler(int signum) {
	(void)signum;
	_signal = true;
}

void	Server::closeFds() {
	for (size_t i = 0; i < _clients.size(); i++) {
		// CLIENT_MSG(RED, "Client", "Client" , _clients[i].getFd(), " has been disconnected.");
		close(_clients[i].getFd());
	}
	if (_fd != -1){
		// MSG(RED, "Server", "Disconnected.");
		close(_fd);
	}
}

void	Server::clearClient(int fd) {
	for (size_t i = 0; i < _fds.size(); i++) {
		if (_fds[i].fd == fd) {
			_fds.erase(_fds.begin() + i);
			break;
		}
	}
	for (size_t i = 0; i < _clients.size(); i++) {
		if (_clients[i].getFd() == fd) {
			_clients.erase(_clients.begin() + i);
			break;
		}
	}
}

