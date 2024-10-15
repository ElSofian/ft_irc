#include <iostream>
#include <vector>
#include <sys/socket.h> 
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <csignal>
#include <stdlib.h>
#include <string.h>

#define WHITE "\033[1;37m"
#define RED "\033[1;31m"
#define GREEN "\e[1;32m"
#define ORANGE "\e[1;33m"
#define BLUE "\e[1;34m"

#define user_id(nickname, username) (":" + nickname + "!" + username + "@localhost")
#define RPL_INVITE(user_id, invited, channel) (user_id + " INVITE " + invited + " #" + channel + "\r\n")
#define MSG(color, prefix, msg) (std::cout << color << "[" << prefix << "] " << WHITE << msg << std::endl)
#define CLIENT_MSG(color, prefix, msg, nb, msg2) (std::cout << color << "[" << prefix << "] " << WHITE << msg << nb << msg2 << std::endl)
#define ERR(msg) (std::cout << RED << "[ERROR] " << WHITE << msg << std::endl)
#define ERR_INVALIDMODEPARAM(client, channel, mode, password) ("696 " + client + " #" + channel + " " + mode + " " + password + " : password must only contained alphabetic character\r\n")