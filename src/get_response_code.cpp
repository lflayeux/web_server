# include "../include/Response.hpp"
# include "../include/Request.hpp"
#include <sys/stat.h>

bool file_exist(const std::string &file_path)
{
	if(access(file_path.c_str(), F_OK | R_OK) == 0)
		return true;
	return false;
}

bool dir_exist(const std::string &file_path)
{
    struct stat info;
    if (stat(file_path.c_str(), &info) != 0)
        return false;

    return (info.st_mode & S_IFDIR) != 0;
}


bool is_allowed_file(const std::string &file_name)
{
    if(file_name =="/")
		return false;
	if(file_name.find("src") != file_name.npos)
		return false;
    if(file_name.find("include") != file_name.npos)
		return false;
	if(file_name.find("cgi-bin") != file_name.npos)
		return false;
	if(file_name.find("build") != file_name.npos)
		return false;
	return true;
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
    else if (code == 302)
        code_message = "Found";        
    else if (code == 404)
        code_message = "Bad Request";                
    else if (code == 403)
        code_message = "Forbidden";
    else if (code == 404)
        code_message = "Not Found";
    else if (code == 405)
        code_message = "Method Not Allowed";
    else if (code == 413)
        code_message = "Payload Too Large";
    else
        code_message = "Default";

    error_code_ = make_pair(code, code_message);
}