#include <fstream>
# include "../../include/webserv.hpp"
# include "../../include/Response.hpp"

int content_length(const char *file_name)
{
	std::ifstream fd(file_name);

	std::string line;
	std::string file_content;
	while(std::getline(fd, line))
		file_content += line + "\n";
	return (file_content.length());
}
void Response::create_path()
{
	std::string real_path;

	if (std::string(get_path_to_send()) == "/")
		real_path = "data/index.html";
	else if (error_code_.first == 404)
		real_path = "data/error/404.html";
	else
		real_path = "data" + get_path_to_send();
	set_path(real_path);
	// std::cout << "FILE TO SEARCH: " << get_path_to_send() << std::endl;
}

std::string Response::create_header()
{
	std::string header;

	// check in the .conf path if there is this file or path
	set_response_code_message();
	create_path();
	std::ostringstream oss;
	oss <<error_code_.first;
	std::string status_code = oss.str();
	header = "HTTP/1.1 " + status_code + " " + error_code_.second + "\r\n";
	std::ostringstream ss;
	// error_code_.first = 0;
	ss << content_length(get_path_to_send().c_str());
	std::string content_len = ss.str();
	header = header + "Content-Length: " + content_len + "\r\n\r\n";

	std::cout << "/t/t/tFILE TO SEND IS : " << get_path_to_send().c_str() << std::endl;
	std::cout << "/t/t/tHEADER IS : " << header << std::endl;
	return (header);
}


std::string Response::response_GET()
{
    std::string msg = create_header();
	std::ifstream index_fd(get_path_to_send().c_str());
    std::string line;

	while(std::getline(index_fd, line))
		msg += line + "\r\n";
	// std::cout << BRED "response :" << msg << std::endl;
    index_fd.close();
    return (msg);
}
