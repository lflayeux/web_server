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
	return (this->path_to_send_);
}

std::string http_request::get_version() const
{
	return (this->version_);
}

void http_request::set_method(int method)
{
	this->method_ = method;
	this->path_to_send_ = "/";
}

void	http_request::set_version(const std::string &version)
{
	this->version_ = version;
}

void	http_request::set_path(const std::string &path)
{
	this->path_to_send_ = path;
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
    size_t start = value_str.find_first_not_of(" \t\r");
    
    if (start == std::string::npos)
    {
        // La ligne ne contient que des espaces après ':'
        this->content_length_ = 0;
        return;
    }
    
    size_t end = value_str.find_last_not_of(" \t\r");
    
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

void	http_request::add_header(const std::string &header, const std::string &value)
{
    this->headers_.insert(std::make_pair(header, value));
}

void	http_request::add_body(const std::string &body)
{
    this->body_ = body;
}

std::ostream &operator<<(std::ostream &flux, http_request const &obj)
{
	std::string	keep_alive;
	
	if (obj.get_keep_alive())
		keep_alive = "keep-alive";
	else
		keep_alive = "keep-dead";


	flux << "METHOD: " << obj.get_method() << " | port: " << obj.get_port()
	<< " | content-length: " << obj.get_content_length() << " | Connection: "
	<< keep_alive << " | version: " << obj.get_version() <<
	" | path: " << obj.get_path_to_send() << std::endl;
	return (flux);
}