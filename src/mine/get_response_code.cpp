# include "../../include/webserv.hpp"
# include "../../include/Response.hpp"

bool file_exist(const std::string &file_path)
{
	if(access(file_path.c_str(), F_OK | R_OK) == 0)
		return true;
	return false;
}


void Response::set_response_code_message(int code)
{
    std::string code_message;
    if (code == 200)
        code_message = "OK";
    else if (code == 201)
        code_message = "Created";
    else if (code == 204)
        code_message = "No Content";                
    else if (code == 404)
        code_message = "Not Found";
    else if (code == 403)
        code_message = "Forbidden";

    error_code_ = make_pair(code, code_message);
}