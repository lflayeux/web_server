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

	while(std::getline(index_fd, line))
		body += line + "\r\n";

	// if (error_code_.first > 299 && error_code_.first < 400)
	// 	msg += "Location: " + getRedirections(get_path_to_send().sub(getRoot().length() - 1), getIdServer(get_port())) + "\r\n";

	msg = create_header(body.length());
	msg += body;

    return (msg);    
}



std::string Response::create_response()
{
    std::string response;

	create_path();

	// else if (is_allowed_method(std::string method))
	// 	set_response_code_message(405);
    if(get_method() == POST)
        response_POST();
    else if(get_method() == GET)
        response_GET();
    else if(get_method() == DELETE)
        response_DELETE();		

	// int conf_error_code = getConfErrorCode(get_path_to_send().sub(getRoot().length() - 1), getIdServer(get_port()));
	// if (conf_error_code != 200)
	// 	set_response_code_message(conf_error_code);
    path_to_error();
    response = format_response();

    return response;
}