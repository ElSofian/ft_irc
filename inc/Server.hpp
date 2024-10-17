# include "Client.hpp"
# include "Channel.hpp"

class Server {
	private:
		int							_fd;
		int 						_port;
		std::string					_password;
		static bool					_signal;
		std::vector<Client>			_clients;
		std::vector<User>			_users;
		std::vector<Channel>		_channels;
		std::vector<struct pollfd>	_fds;

	public:
		Server(int port, std::string password);
		~Server();
		Server(Server const &src);
		Server &operator=(Server const &src);

		int				getPort();
		std::string		getPassword();
		Channel			*getChannel(std::string name);
		const User			*getUserByNickname(std::string nickname, Channel *channel) const;
		User			*getUser(int fd);


		void			init();
		void			createSocket();
		void			acceptNewClient();
		void			receiveData(int fd);
		void			parseData(int fd, std::string data);
		void			sendErrorMessage(int fd, std::string msg);
		void			sendMessage(int fd, std::string msg);
		void			sendMessageToUsers(Channel *chan, std::string authorName, std::string msg);
		void			createServer(std::string channelName, User &user);

		static void 	signalHandler(int signum);

		void			closeFds();
		void			clearClient(int fd);

		void			nick(int fd, std::istringstream &iss);
		void			join(int fd, std::istringstream &iss);
		void			kick(int fd, std::istringstream &iss);
		void			invite(int fd, std::istringstream &iss);
		void			topic(int fd, std::istringstream &iss);
		void			mode(int fd, std::istringstream &iss);

};