#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "webserv.hpp"

class Response : public http_request
{
private:
    std::pair<int, std::string > error_code_;
public:
    Response(){};
    ~Response(){};

    std::string response_GET();
    std::string response_POST();
    std::string response_DELETE();
    std::string create_header();
    void set_response_code_message();
    int get_response_code();
    void create_path();
    void create_response();

};


#endif