# include "../../include/webserv.hpp"
# include "../../include/Response.hpp"

bool file_exist(const std::string &file_path)
{
    std::string root_path = "data";
    std::string full_path = root_path + file_path;
	if(access(full_path.c_str(), F_OK | R_OK) == 0)
		return true;
	return false;
}

int Response::get_response_code()
{
    // std::cout << "PATH BEFORE CHECKING: " << get_path_to_send() << std::endl;
    if(!file_exist(get_path_to_send()))
	{
		return (404);
	}
    return (200);
}

void Response::set_response_code_message()
{
    int response_code = get_response_code(); 
    std::string code_message;
    if (response_code == 200)
        code_message = "OK";
    else if (response_code == 404)
        code_message = "Not Found";

    error_code_ = make_pair(response_code, code_message);
}