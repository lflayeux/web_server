#include "../../include/webserv.hpp"

http_request *create_test()
{
    http_request *request = new http_request();
    request->set_method(GET);
    request->set_path("/lolo.html");
    request->set_port(8080);
    request->set_keep_alive(true);
    request->set_version("HTTP/1.1");

    return (request);
}
