#include "../../include/Response.hpp"

int Response::response_code_DELETE()
{
    // std::cout << "PATH BEFORE CHECKING: " << get_path_to_send() << std::endl;
    if(!file_exist(get_path_to_send()))
		return (404);
    return (200);
}

void Response::response_DELETE()
{
    // set_path("data" + get_path_to_send());
	set_response_code_message(response_code_DELETE());
    std::remove(get_path_to_send().c_str());
}
