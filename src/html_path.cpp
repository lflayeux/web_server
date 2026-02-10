#include "../include/Response.hpp"

void Response::path_to_error()
{
	std::string error_page = getErrorPage(error_code_.first, getIdServer(get_port()));
	if (error_page != "default")
	{
		set_path(error_page);
		return ;
	}
	if (error_code_.first == 404)
		set_path("data/error/404.html");
	else if (error_code_.first == 403)
		set_path("data/error/403.html");        
	else if (error_code_.first == 201 && get_method() == POST)
		set_path("data/uploads.html");
	else if (error_code_.first >= 400)
		set_path("data/error/default_error.html");
}