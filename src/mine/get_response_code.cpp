# include "../../include/webserv.hpp"
# include "../../include/Response.hpp"

bool file_exist(const std::string &file_path)
{
	if(access(file_path.c_str(), F_OK | R_OK) == 0)
		return true;
	return false;
}

bool is_allowed_file(const std::string &file_name)
{
    if(file_name.find("data/uploads/") == 0)
		return true;
	return false;
}

// bool Response::is_allowed_method(std::string method)
// {
//     std::vector<std::string> method_allowed = getMethodAllowed(get_path_to_send().sub(getRoot().length() - 1), getIdServer(get_port()));

//     if(method_allowed.empty())
//         return true;
//     if (std::find(method_allowed.begin(), method_allowed.end(), method) != method_allowed.end())
//         return true;
//     return false;
// }

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
        code_message = "Bad Request";                
    else if (code == 403)
        code_message = "Forbidden";
    else if (code == 404)
        code_message = "Not Found";
    else if (code == 405)
        code_message = "Method Not Allowed";
    else
        code_message = "Default";

    error_code_ = make_pair(code, code_message);
}