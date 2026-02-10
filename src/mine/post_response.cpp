#include "../../include/webserv.hpp"
#include "../../include/Response.hpp"

// CHECK AND CONSIDER PRG TECHNIC TO AVOID REFRESHING AND RESENDING DATA
// int Response::response_code_POST()
// {

// }

void Response::response_POST()
{
    std::string upload_location = getUploadLocation(get_path_to_send(), getIdServer(get_port()));

    if(getUploadAllowed(get_path_to_send(), getIdServer(get_port())) == false)
        set_response_code_message(405);
    else if(!file_exist(upload_location))
        set_response_code_message(404);
    else if (!is_allowed_file(upload_location))
        set_response_code_message(403);
    else
        set_response_code_message(302);


    std::cout << "frffffffffffffffffffffffffffff ERROR CODE: " << upload_location << std::endl;
    std::cout << "frffffffffffffffffffffffffffff ERROR CODE: " << error_code_.first << std::endl;
    if (error_code_.first == 302)
    {
        std::ofstream index_fd(upload_location.c_str() , std::ios::app);
        index_fd << get_body() + "\n";
    }
}



