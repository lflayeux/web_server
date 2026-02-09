#include "../../include/webserv.hpp"
#include "../../include/Response.hpp"

// CHECK AND CONSIDER PRG TECHNIC TO AVOID REFRESHING AND RESENDING DATA
int Response::response_code_POST()
{
    // if(getUploadAllowed(get_path_to_send().sub(getRoot().length() - 1), getIdServer(get_port())) == false)
    //     return (405);
    if(!file_exist(get_path_to_send()))
		return (404);
    else if (!is_allowed_file(get_path_to_send()))
        return (403);
    return (201);
}

void Response::response_POST()
{
    // std::string upload_location = getUploadLocation(get_path_to_send().sub(getRoot().length() - 1), getIdServer(get_port()));
	set_response_code_message(response_code_POST());
    if (error_code_.first == 201)
    {
        // std::ofstream index_fd(get_upload_location.c_str() , std::ios::app);
        std::ofstream index_fd("data/uploads/data.txt",std::ios::app);
        index_fd << get_body() + "\n";
    }
}



