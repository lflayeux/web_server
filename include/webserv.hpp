#ifndef WEBSERV_HPP
#define WEBSERV_HPP

# include "colors.hpp"
# include <sys/socket.h>
# include <sys/epoll.h>
# include <netdb.h>
# include <string.h>
# include <string>
# include <fcntl.h>
# include <unistd.h>
# include <stdio.h>
# include <map>
# include <sstream>
# include <iostream>
# include <iomanip>
#include <unistd.h>

std::string get_response(const char *file_path);

enum    method
{
    POST,
    GET,
    DELETE,
};

class http_request
{
    public:
        http_request(){};
        ~http_request(){};
        int            get_method() const;
        std::string    get_version() const;
        int            get_port() const;
        int            get_content_length() const;
        bool        get_keep_alive() const;
        std::string get_path_to_send() const;
        void        set_method(int method);
        void         set_path(const std::string &path);
        void         set_version(const std::string &version);
        void        set_port(int port);
        void        set_content_length(const std::string &line);
        void        set_keep_alive(bool status);
    private:
        bool        keep_alive_;
        int            method_;
        std::string    version_;
        int            port_;
        int            content_length_;
        std::string    path_to_send_;
};

std::ostream &operator<<(std::ostream &flux, http_request const &obj);
std::string create_header(http_request &request);
std::pair<int, std::string > get_response_code_message(http_request &request);

http_request *create_test();


#endif