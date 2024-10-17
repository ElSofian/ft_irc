# include "defines.hpp"
# include "User.hpp"

class Client {
	private:
		int			_fd;
		std::string	_ip;
		User		_user;
		bool		_isRegistered;

	public:
		Client();
		~Client();
		Client(Client const &src);
		Client &operator=(Client const &src);

		int			getFd();
		std::string	getIp();
		User		getUser();
		bool		getIsRegistered();

		void		setFd(int fd);
		void		setIp(std::string ip);
		void		setUser(User *user);
		void		setIsRegistered(bool isRegistered);
};