# include "defines.hpp"

class User {
	private:
		int			_fd;
		std::string _nickname;
		std::string _username;
		std::string _password;
		
	public:
		User();
		~User();
		User(User const &src);
		User &operator=(User const &src);

		int			getFd();
		std::string	getNickname();
		std::string	getUsername();
		std::string	getPassword();

		void		setFd(int fd);
		void		setNickname(std::string nickname);
		void		setUsername(std::string username);
		void		setPassword(std::string password);
};