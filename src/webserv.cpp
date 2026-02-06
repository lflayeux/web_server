#include "../include/webserv.hpp"

int	http_request::get_method() const
{
	return (this->method_);
}

int	http_request::get_port() const
{
	return (this->port_);
}

std::string http_request::get_path_to_send() const
{
	return this->path_to_send;
}

void http_request::set_method(int method)
{
	this->method_ = method;
	this->path_to_send = "/";
}

void http_request::set_method_Get(std::string line)
{
	this->method_ = GET;
	std::string path;
	size_t firstSpace = line.find(' ');
    if (firstSpace != std::string::npos) 
	{
    	size_t secondSpace = line.find(' ', firstSpace + 1);
        if (secondSpace != std::string::npos) 
		{
            path = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
            std::cout << "Le path demande est : " << path << std::endl;
        }
    }
	path_to_send = path;
}

void http_request::set_port(int port)
{
	this->port_ = port;
}

std::ostream &operator<<(std::ostream &flux, http_request const &obj)
{
	flux << "METHOD: " << obj.get_method() << " | " << obj.get_port() << std::endl;
	return (flux);
}