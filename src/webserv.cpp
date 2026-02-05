#include "../include/webserv.hpp"

int	http_request::get_method() const
{
	return (this->method_);
}

int	http_request::get_port() const
{
	return (this->port_);
}

int	http_request::get_content_length() const
{
	return (this->content_length_);
}

bool	http_request::get_keep_alive() const
{
	return (this->keep_alive_);
}

std::string http_request::get_path_to_send() const
{
	return this->path_to_send_;
}

void http_request::set_method(int method)
{
	this->method_ = method;
	this->path_to_send_ = "/";
}

void	http_request::set_method_Get(std::string line)
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
	path_to_send_ = path;
}

void	http_request::set_keep_alive(bool status)
{
	this->keep_alive_ = status;
}

void http_request::set_content_length(const std::string &line)
{
	size_t pos_sep = line.find(':');
	if (pos_sep == std::string::npos)
	{
		this->content_length_ = 0;
		return;
	}

	// Récupère tout ce qu'il y a après ':'
	std::string value_str = line.substr(pos_sep + 1);

	// Trim espaces et \r
	size_t start = value_str.find_first_not_of(" \t");
	size_t end   = value_str.find_last_not_of(" \t\r");

	if (start == std::string::npos || end == std::string::npos)
	{
		this->content_length_ = 0;
		return;
	}

	value_str = value_str.substr(start, end - start + 1);

	// Conversion manuelle string -> int
	int value = 0;
	for (size_t i = 0; i < value_str.length(); ++i)
	{
		if (value_str[i] < '0' || value_str[i] > '9')
		{
			this->content_length_ = 0;
			return;
		}
		value = value * 10 + (value_str[i] - '0');
	}

	this->content_length_ = value;

	std::cout << "Le content length demandé est : "
	          << this->content_length_ << std::endl;
}


void http_request::set_port(int port)
{
	this->port_ = port;
}

std::ostream &operator<<(std::ostream &flux, http_request const &obj)
{
	std::string	keep_alive;
	
	if (obj.get_keep_alive())
		keep_alive = "keep-alive";
	else
		keep_alive = "keep-dead";


	flux << "METHOD: " << obj.get_method() << " | port: " << obj.get_port() <<
	" | content-length: " << obj.get_content_length() << " | Connection: " << keep_alive << std::endl;
	return (flux);
}