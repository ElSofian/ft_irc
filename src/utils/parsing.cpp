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
            if (command == "NICK" || command == "USER") {
                std::string output;
                iss >> output;
                command == "NICK" ? user->setNickname(output) : user->setUsername(output);
            } else if (command != "CAP" && command != "PASS") {
				ERR("Unknown command: " + command);
				return ;
			}

		}
    }
}
