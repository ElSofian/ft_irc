# include "Server.hpp"

void Server::parseData(int fd, std::string data) {
    std::string delimiter = "\r\n";
    size_t pos = 0;
    std::string line;
    static bool welcomeMsgSent = false;

    while ((pos = data.find(delimiter)) != std::string::npos) {
        line = data.substr(0, pos);
        data.erase(0, pos + delimiter.length());

        if (!line.empty()) {
            std::istringstream iss(line);
            std::string command;
            iss >> command;

			User *user = getUser(fd);
            if (command == "NICK" || command == "USER") {
                std::string output;
                iss >> output;
                command == "NICK" ? user->setNickname(output) : user->setUsername(output);
                if (!welcomeMsgSent)
                {
                    sendMessage(fd, "001 Welcome to the server " + user->getNickname() + "!\r\n");
                    welcomeMsgSent = true;
                }
            } else if (command == "JOIN") {
                std::string channelName;
                iss >> channelName;
                if (channelName.empty()) {
                    sendMessage(fd, "461 JOIN :Not enough parameters\r\n");
                    return;
                }

                Channel *channel = getChannel(channelName);
                if (!channel) {
                    Channel newChannel(channelName, "");
                    _channels.push_back(newChannel);
                    channel = &_channels.back();
                }

                channel->addUser(*user);
                sendMessage(fd, "JOIN " + channelName + "\r\n");
            }

		}
    }
}
