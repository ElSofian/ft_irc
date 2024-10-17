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

Channel *Server::getChannel(std::string name) {
	for (size_t i = 0; i < _channels.size(); i++) {
		if (_channels[i].getChannelName() == name)
			return &_channels[i];
	}
	return NULL;
}

User	*Server::getUser(int fd) {
	for (size_t i = 0; i < _users.size(); i++) {
		if (_users[i].getFd() == fd)
			return &_users[i];
	}
	return NULL;
}

const User	*Server::getUserByNickname(std::string nickname, Channel *channel) const {
	for (size_t i = 0; i < channel->getMembersList().size(); i++) {
		if (channel->getMembersList()[i].getNickname() == nickname)
			return &channel->getMembersList()[i];
	}
	return NULL;
}

void	Server::init() {
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGPIPE, signalHandler);
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

void	Server::sendErrorMessage(int fd, std::string msg) {
	const std::string errorMsg = std::string(RED) + "[ERROR] " + WHITE + msg + "\r\n";
	sendMessage(fd, errorMsg);
}

void	Server::sendMessage(int fd, std::string msg) {
	int 		bytes;
	int 		dataSent = 0;
	
	if (msg.size() > 512) {
		std::cout << "Warning : Message sent is more than 512 characters, truncating the result" << std::endl;
		msg = msg.substr(0, 510);
		msg += "\r\n";
		std::cout << "Warning : message is now [" << msg << "]" << std::endl;
	}
	
	while (size_t(dataSent) < msg.size())
	{
		if ((bytes = send(fd, msg.c_str() + dataSent, msg.size() - dataSent, MSG_DONTWAIT)) <= 0)
		{
			sendErrorMessage(fd, "451 :Client where the message is supposed to be sent is not registered.\r\n");
			return ;
		}
		dataSent += bytes;
	}
}

void	Server::sendMessageToUsers(Channel *chan, std::string authorName, std::string msg) {
	for (size_t i = 0; i < chan->getMembersList().size(); i++) {
		if (chan->getMembersList()[i].getNickname() == authorName)
			continue;
		sendMessage(chan->getMembersList()[i].getFd(), msg);
	}
}

void	Server::createServer(std::string channelName, User &user) {
	Channel newChannel(channelName, "");
	_channels.push_back(newChannel);
	newChannel.addUser(user);
	// sendMessageToUsers(&newChannel, user.getNickname(), RPL_JOIN(USER(user.getNickname(), user.getUsername()), newChannel.getChannelName()));
	std::string joinMessage = RPL_TOPIC(user.getNickname(), newChannel.getChannelName(), newChannel.getChannelTopic())		+ 
									RPL_NAMREPLY(user.getNickname(), newChannel.getChannelName(), newChannel.getMembersListNames())	+ 
									RPL_ENDOFNAMES(user.getNickname(), newChannel.getChannelName());
	sendMessage(user.getFd(), joinMessage);
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
	// sendMessage(newClientFd, "001 : Welcome to the server !\r\n");
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
		// std::cout << "\n-------------------\nraw data:\n" << buff << "\n------------------------\n" << std::endl;
		
		User *user = getUser(fd);
		if (!user)
			return ;
		
		parseData(fd, buff);
	}
}

void	Server::signalHandler(int signum) {
	if (signum == SIGPIPE)
		return ;
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

