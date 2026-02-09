#include "../../include/Response.hpp"

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
    // set_path("data" + get_path_to_send());
	set_response_code_message(response_code_DELETE());
    if (error_code_.first == 204)
        std::remove(get_path_to_send().c_str());
}
