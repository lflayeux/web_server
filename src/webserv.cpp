#include "../include/webserv.hpp"

int	http_request::get_method() const
{
	return (this->method_);
}

int	http_request::get_port() const
{
	return (this->port_);
}

void http_request::set_method(int method)
{
	this->method_ = method;
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