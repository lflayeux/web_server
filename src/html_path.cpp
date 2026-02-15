#include "../include/Response.hpp"

void Response::path_to_error()
{
	std::string error_page = getErrorPage(error_code_.first, getIdServer(getHostName(),get_port()));
	if (error_page != "default")
	{
		set_path(error_page);
		return ;
	}
	if (error_code_.first == 404)
		set_path("src/data/default_pages/404.html");
	else if (error_code_.first == 403)
		set_path("src/data/default_pages/403.html");        
	else if (error_code_.first == 201 && get_method() == "POST")
		set_path("src/data/default_pages/uploads.html");
	else if (error_code_.first >= 400)
		set_path("src/data/default_pages/default_error.html");
}