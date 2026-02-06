# include "../../include/webserv.hpp"

bool file_exist(const std::string &file_path)
{
	if(access(file_path.c_str(), F_OK | R_OK) == 0)
		return true;
	return false;
}

int get_response_code(http_request &request)
{
    if(!file_exist(request.get_path_to_send()))
	{
		return (404);
	}
    return (200);
}

std::pair<int, std::string > get_response_code_message(http_request &request)
{

    int response_code = get_response_code(request); 
    std::string code_message;
    if (response_code == 200)
        code_message = "OK";
    else if (response_code == 404)
        code_message = "Not Found";

    return (std::make_pair(response_code, code_message));
}