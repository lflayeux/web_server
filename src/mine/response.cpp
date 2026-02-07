#include "../../include/Response.hpp"

void Response::create_response()
{
    if (get_method() == GET)
        response_GET();
    else if(get_method() == POST)
        response_POST();
    else if(get_method() == DELETE)
        response_DELETE();
}