#ifndef REQUEST_HPP
#define REQUEST_HPP

# include "Config.hpp"
# include "colors.hpp"
# include <sys/socket.h>
# include <sys/epoll.h>
# include <netdb.h>
# include <string.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdio.h>
# include <map>
# include <vector>
# include <sstream>
# include <iostream>
# include <iomanip>
# include <cstdlib>
# include <cerrno>
# include <sstream>
# include <cstring>
# include <sys/wait.h>

std::string get_response(const char *file_path);

enum	method
{
	POST,
	GET,
	DELETE,
};

class Request : public Config
{
	public:
		Request(){};
		~Request(){};
		int			get_id_server() const;
		std::string	get_method() const;
		std::string	get_version() const;
		int			get_port() const;
		int			get_content_length() const;
		bool		get_keep_alive() const;
		std::string get_path_to_send() const;
		std::string get_body() const;
		bool		is_cgi_request() const;
		std::map<std::string, std::string>	get_headers() const;
		void		set_method(std::string method);
		void 		set_path(const std::string &path);
		void 		set_version(const std::string &version);
		void		set_port(int port);
		void		set_content_length(const std::string &line);
		void		set_keep_alive(bool status);
		void		add_header(const std::string &header, const std::string &value);
		void		add_body(const std::string &body);
	private:
		int			id_server_;
		bool		keep_alive_;
		std::string	method_;
		std::string	version_;
		int			port_;
		int			content_length_;
		std::string	path_to_send_;
		std::string	body_;

		std::map<std::string, std::string>	headers_;
};

std::ostream &operator<<(std::ostream &flux, Request const &obj);

#endif