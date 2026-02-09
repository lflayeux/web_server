#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "webserv.hpp"
#include <dirent.h>
#include <sys/stat.h>

class Response : public http_request
{
private:
    std::pair<int, std::string > error_code_;
public:
    Response(){};
    ~Response(){};

    void response_GET();
    void response_POST();
    void response_DELETE();
    std::string create_header(int content_length);
    void set_response_code_message(int code);
    void create_path();
    void path_to_error();
    std::string create_response();

    int response_code_POST();
    int response_code_GET();
    int response_code_DELETE();

    std::string format_response();

    // bool is_allowed_method(int method);

};

bool file_exist(const std::string &file_path);
bool is_allowed_file(const std::string &file_name);



#endif