#ifndef REQUEST_HPP
#define REQUEST_HPP

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
# include <cstdlib>

std::string get_response(const char *file_path);

enum	method
{
	POST,
	GET,
	DELETE,
};

/* TO TEST before merge */
class	Config
{
	protected:
		int	nb_serv_;
};

class Request : public Config
{
	public:
		Request(){};
		~Request(){};
		int			get_id_server() const;
		int			get_method() const;
		std::string	get_version() const;
		int			get_port() const;
		int			get_content_length() const;
		bool		get_keep_alive() const;
		std::string get_path_to_send() const;
		std::string get_body() const;
		void		set_id_serv_();
		void		set_method(int method);
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
		int			method_;
		std::string	version_;
		int			port_;
		int			content_length_;
		std::string	path_to_send_;
		std::string	body_;

		std::map<std::string, std::string>	headers_;
};

std::ostream &operator<<(std::ostream &flux, Request const &obj);

#endif