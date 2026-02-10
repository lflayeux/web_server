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

/* TO TEST before merge */
class	Config
{
	protected:
		int	nb_serv_;
		std::string	script_name_;
		std::string	cgi_path_;
		std::vector<std::string> cgi_extensions_;
	public:
		Config() : nb_serv_(2), script_name_("test.py"), cgi_path_("www/cgi-bin/")
		{
			cgi_extensions_.push_back(".php");
			cgi_extensions_.push_back(".py");
			cgi_extensions_.push_back(".pl");
		};
		~Config(){};
		std::string	get_cgi_path() const
		{
			return cgi_path_;
		};
		std::vector<std::string> get_cgi_extensions() const
		{
			return cgi_extensions_;
		};		
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
		std::string	get_script_name() const;
		bool		is_cgi_request() const;
		std::map<std::string, std::string>	get_headers() const;
		void		set_id_serv_();
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