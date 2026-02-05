#ifndef WEBSERV_HPP
#define WEBSERV_HPP

# include "colors.hpp"
# include <sys/socket.h>
# include <sys/epoll.h>
# include <netdb.h>
# include <string.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdio.h>
# include <map>
# include <sstream>
# include <iostream>
# include <iomanip>

std::string get_response(const char *file_path);

enum	method
{
	POST,
	GET,
	DELETE,
};

class http_request
{
	public:
		http_request(){};
		~http_request(){};
		int		get_method() const;
		int		get_port() const;
		std::string get_path_to_send() const;
		void	set_method(int method);
		void 	set_method_Get(std::string line);
		void	set_port(int port);
	private:
		int				method_;
		int				port_;
		std::string		path_to_send;
};

std::ostream &operator<<(std::ostream &flux, http_request const &obj);

#endif