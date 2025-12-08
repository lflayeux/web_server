#include <fstream>

std::string get_response(const char *file_path)
{
    std::ifstream index_fd(file_path);
    std::string line;
    std::string msg = "";
    while(std::getline(index_fd, line))
        msg += line + "\r\n";
    index_fd.close();
    return (msg);
}
