#include "Client.hpp"

class Server {
	private:
		int							_fd;
		int 						_port;
		std::string					_password;
		static bool					_signal;
		std::vector<Client>			_clients;
		std::vector<struct pollfd>	_fds;

	public:
		Server(int port, std::string password);
		~Server();
		Server(Server const &src);
		Server &operator=(Server const &src);

		int				getPort();
		std::string		getPassword();

		void			init();
		void			createSocket();
		void			acceptNewClient();
		void			sendMessage(int fd, std::string msg);
		void			receiveData(int fd);
		void			parseData(int fd, std::string data);

		static void 	signalHandler(int signum);

		void			closeFds();
		void			clearClient(int fd);
};