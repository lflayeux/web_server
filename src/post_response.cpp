#include "../include/Request.hpp"
#include "../include/Response.hpp"

void Response::response_POST()
{
    std::string upload_location = getUploadLocation(get_path_to_send(), getIdServer(getHostName(),get_port()));

    if(getUploadAllowed(get_path_to_send(), getIdServer(getHostName(),get_port())) == false)
        set_response_code_message(405);
    else if(!dir_exist(upload_location))
        set_response_code_message(404);
    else if (!is_allowed_file(upload_location))
        set_response_code_message(403);
    else
        set_response_code_message(302);


    std::cout << "upload_lacation: " << upload_location << std::endl;
    std::cout << BRED << "ERROR CODE: " << error_code_.first << RESET << std::endl;
    if (error_code_.first == 302)
    {
		std::string file_location;
		if (upload_location[upload_location.size() - 1] != '/')
			upload_location += "/";
		file_location = upload_location + get_headers()["filename"];
		std::cerr << BRED << "FILE_NAME LOCATION: " << file_location << RESET << std::endl;
        std::ofstream index_fd(file_location.c_str());
        index_fd << get_body();
    }
}



