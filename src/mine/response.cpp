#include "../../include/Response.hpp"

int content_length(const char *file_name)
{
	std::ifstream fd(file_name);

	std::string line;
	std::string file_content;
	while(std::getline(fd, line))
		file_content += line + "\n";
	return (file_content.length());
}

std::string Response::create_header(int content_length)
{
	std::string header;

	// check in the .conf path if there is this file or path

	std::ostringstream oss;
	oss << error_code_.first;
	std::string status_code = oss.str();
	header = "HTTP/1.1 " + status_code + " " + error_code_.second + "\r\n";
	std::ostringstream ss;
	// error_code_.first = 0;
	header += "Content-Type: text/html\r\n";
	std::cout << "/t/t/tFILE TO SEND IS : " << get_path_to_send().c_str() << std::endl;
	ss << content_length;
	std::string content_len = ss.str();
	header = header + "Content-Length: " + content_len;
	header += "\r\n\r\n";

	std::cout << "/t/t/tMETHOD IS : " << get_method() << std::endl;
	std::cout << "/t/t/tHEADER IS : " << header << std::endl;
	return (header);
}

std::string Response::format_response()
{
    std::string msg;
    std::string body = "";
	std::ifstream index_fd(get_path_to_send().c_str());
    std::string line;

	std::cout << BRED "PATH TO SEND HEHHAHAHAHAH:" << get_path_to_send() << std::endl;
	while(std::getline(index_fd, line))
	{
		body += line + "\r\n";
	}
	std::cout << BRED "BODY LENGTH HEHHAHAHAHAH:" << body.length() << std::endl;
	msg = create_header(body.length());
	msg += body;
	// std::cout << BRED "response :" << msg << std::endl;
    return (msg);    
}

std::string Response::create_response()
{
    std::string response;

	create_path();
    if(get_method() == POST)
        response_POST();
    else if(get_method() == GET)
        response_GET();
    else if(get_method() == DELETE)
        response_DELETE();

    path_to_error();
    response = format_response();

    return response;
}