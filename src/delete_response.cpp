#include "../include/Response.hpp"

int Response::response_code_DELETE()
{
    if(!file_exist(get_path_to_send()))
		return (404);
    else if (!is_allowed_file(get_path_to_send()))
        return (403);
    return (204);
}

void Response::response_DELETE()
{
    std::string root_dir = getRoot(get_path_to_send(), getIdServer(getHostName(), get_port()));
    std::string full_path = root_dir + get_path_to_send();
    std::cout << BRED "------------------- PATH TO FIND : " << root_dir + get_path_to_send() << std::endl;

    if(!file_exist(full_path))
		set_response_code_message(404);
    else if (!is_allowed_file(full_path))
        set_response_code_message(403);
    else
        set_response_code_message(204);

    if (error_code_.first == 204)
        std::remove(full_path.c_str());
}
