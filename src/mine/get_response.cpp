#include <fstream>
# include "../../include/webserv.hpp"

int content_length(const char *file_name)
{
	std::ifstream fd(file_name);

	std::string line;
	std::string file_content;
	while(std::getline(fd, line))
		file_content += line + "\n";
	return (file_content.length());
}

std::string create_header(http_request &request)
{
	std::string header;


	std::pair<int, std::string > response_type = get_response_code_message(request);
	std::ostringstream oss;
	oss << response_type.first;
	std::string status_code = oss.str();
	header = "HTTP/1.1 " + status_code + " " + response_type.second + "\r\n";
	std::ostringstream ss;
	ss << content_length(request.get_path_to_send().c_str());
	std::string content_len = ss.str();
	header = header + "Content-Length: " + content_len + "\r\n\r\n";

	std::cout << "/t/t/tFILE TO SEND IS : " << request.get_path_to_send().c_str() << std::endl;
	std::cout << "/t/t/tHEADER IS : " << header << std::endl;
	return (header);
}


std::string get_response(const char *file_path)
{
	std::string real_path;
	// check in the .conf path if there is this file or path
	if (std::string(file_path) == "/")
		real_path = "index.html";
	else
	{
		real_path = file_path;
		real_path = real_path.substr(1);
	}
	std::cout << BRED "real path :" << real_path << std::endl;
	std::ifstream index_fd(real_path.c_str());
    std::string line;
    std::string msg = create_header(*create_test());

	while(std::getline(index_fd, line))
		msg += line + "\r\n";

	// std::cout << BRED "response :" << msg << std::endl;
    index_fd.close();
    return (msg);
}
