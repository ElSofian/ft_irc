# include "Server.hpp"

void	Server::parseData(int fd, std::string data) {
	std::string line;
	std::istringstream stream(data);
	
	while (std::getline(stream, line)) {
		if (line.find("NICK") != std::string::npos) {
			// handle NICK command
		} else if (line.find("USER") != std::string::npos) {
			
		} else if (line.find("JOIN") != std::string::npos) {
			// handle JOIN command
		} else if (line.find("PART") != std::string::npos) {
			// handle PART command
		} else if (line.find("PRIVMSG") != std::string::npos) {
			// handle PRIVMSG command
		} else if (line.find("QUIT") != std::string::npos) {
			// handle QUIT command
		} else if (line.find("PING") != std::string::npos) {
			
		} else if (line.find("PONG") != std::string::npos) {
			// handle PONG command
		} else {
			ERR("Invalid command");
			return;
		}
	}
}