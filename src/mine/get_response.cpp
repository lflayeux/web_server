#include <fstream>
# include "../../include/webserv.hpp"
# include "../../include/Response.hpp"

int Response::response_code_GET()
{
    if(!file_exist(get_path_to_send()))
		return (404);
    return (200);
}

void Response::create_path()
{
	if (std::string(get_path_to_send()) == "/")
		set_path("data/index.html");
	else
		set_path("data" + get_path_to_send());
	// std::cout << "FILE TO SEARCH: " << get_path_to_send() << std::endl;
}

void Response::path_to_error()
{
	if (error_code_.first == 404)
		set_path("data/error/404.html");
	else if (error_code_.first == 200 && get_method() == POST)
		set_path("data/uploads.html");
}


void Response::response_GET()
{
	set_response_code_message(response_code_GET());
}
