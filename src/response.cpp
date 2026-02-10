#include "../include/Response.hpp"

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

	std::ostringstream oss;
	oss << error_code_.first;
	std::string status_code = oss.str();
	header = "HTTP/1.1 " + status_code + " " + error_code_.second + "\r\n";
	std::ostringstream ss;



	// CHECK FOR REDIRECTIONS OF POST OR DELETE THAT NEEDS A DEFAULT PATH FOR REDIRECTIONS NOT ONLY THE REDIR FROM CONFIG
	if (error_code_.first > 299 && error_code_.first < 400)
	{
		int conf_error_code = getConfErrorCode(get_path_to_send(), getIdServer(get_port()));
		std::string redirection = getRedirections(get_path_to_send(), getIdServer(get_port()));
		if (conf_error_code == error_code_.first && !redirection.empty())
			header += "Location: " + redirection + "\r\n";
		else
			header += "Location: /uploads.html\r\n";
	}
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

	msg = create_header(body.length());
	msg += body;

    return (msg);    
}

std::string Response::create_response()
{
	std::string response;
	int conf_error_code = getConfErrorCode(get_path_to_send(), getIdServer(get_port()));
	std::cout << "------------------ CONF ERROR CODE FOR METHOD " << get_method() << " | " << conf_error_code << "\n";

	if(getMaxBodySize(get_path_to_send(), getIdServer(get_port())) < static_cast<long>(get_body().length()))
		set_response_code_message(413);
	else if (conf_error_code != 200 && (get_path_to_send().find('.') == get_path_to_send().npos || get_method() == DELETE))
		set_response_code_message(conf_error_code);
	else
	{
		// MANQUE LE get_root() de LUCAS
		// if (!isMethodAllowed(get_path_to_send(), getIdServer(get_port()), get_method()))
		// 	set_response_code_message(405);			
		if(get_method() == POST)
			response_POST();
		else if(get_method() == GET)
			response_GET();
		else if(get_method() == DELETE)
			response_DELETE();		
	}
	
	path_to_error();

	response = format_response();

    return response;
}