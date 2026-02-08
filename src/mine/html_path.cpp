#include "../../include/Response.hpp"

void Response::path_to_error()
{
	if (error_code_.first == 404)
		set_path("data/error/404.html");
	else if (error_code_.first == 403)
		set_path("data/error/403.html");        
	else if (error_code_.first == 201 && get_method() == POST)
		set_path("data/uploads.html");
	else if (error_code_.first == 204 && get_method() == DELETE)
		set_path("data/delete.html");
}