#include <fstream>
# include "../include/Request.hpp"


std::string get_response(const char *file_path)
{
	std::string real_path;
	if (std::string(file_path) == "/")
		real_path = "index.html";
	else
	{
		real_path = file_path;
		real_path = real_path.substr(1);
	}
	std::cout << BRED "real path :" << real_path << std::endl;
	std::ifstream index_fd(real_path.c_str());
    std::string line;
    std::string msg = "";
    while(std::getline(index_fd, line))
        msg += line + "\r\n";
    index_fd.close();
    return (msg);
}
