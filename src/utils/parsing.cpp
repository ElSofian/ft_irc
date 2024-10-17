# include "Server.hpp"

void Server::parseData(int fd, std::string data) {
    std::string delimiter = "\r\n";
    size_t pos = 0;
    std::string line;

    while ((pos = data.find(delimiter)) != std::string::npos) {
        line = data.substr(0, pos);
        data.erase(0, pos + delimiter.length());

        if (!line.empty()) {
            std::istringstream iss(line);
            std::string command;
            iss >> command;

			User *user = getUser(fd);
            if (!user) {
                sendErrorMessage(fd, "451 :You're not registered\r\n");
                return ;
            }
            
            if (command == "NICK" || command == "USER") {
                std::string output;
                iss >> output;
                if (command == "NICK") {
                    user->setNickname(output);
                    sendMessage(fd, "Your nickname has been set to " + user->getNickname() + "!\r\n");
                } else {
                    user->setUsername(output);
                    sendMessage(fd, "Your username has been set to " + user->getUsername() + "!\r\n");
                }
            } else if (command == "KICK") {
                kick(fd, iss);
            } else if (command == "INVITE") {
                invite(fd, iss);
            } else if (command == "TOPIC") {
                topic(fd, iss);
            } else if (command == "PONG") {
                std::string output;
                iss >> output;
                sendMessage(fd, "PONG " + output + "\r\n");
            }
            // else if (command == "MODE") {
            //     mode(fd, iss);
            // }

		}
    }
}
