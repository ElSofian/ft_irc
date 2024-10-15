# include "defines.hpp"

class Client {
	private:
		int			_fd;
		std::string	_ip;
		std::string	_nickname;
		std::string	_username;
		std::string	_password;

	public:
		Client();
		~Client();
		Client(Client const &src);
		Client &operator=(Client const &src);

		int			getFd();
		std::string	getIp();

		void		setFd(int fd);
		void		setIp(std::string ip);
};