# include "Utils.hpp"
# include "Server.hpp"

void	Server::kick(int fd, std::istringstream &iss) {
	std::string user;
	std::string channel;

	iss >> user;
	iss >> channel;

	Channel *chan = getChannel(channel);
	if (chan == NULL) {
		sendErrorMessage(fd, "403 " + channel + " :No such channel\r\n");
		return ;
	}

	sendMessage(fd, "441 " + user + " " + channel + "\r\n");
}

void   Server::invite(int fd, std::istringstream &iss) {
	std::string user;
	std::string channel;

	iss >> user;
	iss >> channel;

	Channel *chan = getChannel(channel);
	if (chan == NULL) {
		sendErrorMessage(fd, "403 " + channel + " :No such channel\r\n");
		return ;
	}

	sendMessage(fd, "341 " + user + " " + channel + "\r\n");
}

void   Server::topic(int fd, std::istringstream &iss) {
	std::string channel;
	std::string topic;

	iss >> channel;
	iss >> topic;

	std::cout << "Channel: " << channel << " | Topic: " << topic << std::endl;

	Channel *chan = getChannel(channel);
	if (chan == NULL) {
		sendErrorMessage(fd, "403 " + channel + " :No such channel\r\n");
		return ;
	}

	chan->setChannelTopic(topic);
	sendMessage(fd, "332 " + channel + " :" + topic + "\r\n");
}

void   Server::mode(int fd, std::istringstream &iss) {
	// set the mode (i, t, k, o, and l only)

	std::string channel;
	std::string mode;
	std::string password;

	iss >> channel;
	iss >> mode;
	iss >> password;

	Channel *chan = getChannel(channel);
	if (chan == NULL) {
		sendErrorMessage(fd, "403 " + channel + " :No such channel\r\n");
		return ;
	}

	if (mode == "i" || mode == "t" || mode == "k" || mode == "o" || mode == "l") {
		if (mode == "k" && !check(atoi(password.c_str()), password)) {
			std::string client = USER(std::string("server"), std::string("server"));
			sendMessage(fd, ERR_INVALIDMODEPARAM(client, channel, mode, password));
			return ;
		}
		chan->addMode(mode);
		sendMessage(fd, "324 " + channel + " " + mode + "\r\n");
	} else {
		sendMessage(fd, "472 " + mode + " :is unknown mode char to me\r\n");
	}
}