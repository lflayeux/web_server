#include "../../include/webserv.hpp"
#include "../../include/Response.hpp"

int Response::response_code_POST()
{
    return (200);
}

void Response::response_POST()
{
    std::ofstream index_fd("data/uploads/data.txt",std::ios::app);
    index_fd << get_body() + "\n";
	set_response_code_message(response_code_POST());
}



