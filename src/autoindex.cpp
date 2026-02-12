#include "../include/Response.hpp"

std::string create_page(std::vector<std::pair<std::string, int> > file_list)
{
    std::string autoindex_html = "<html>\r\n<head><title>Index /</title></head>\r\n";
    autoindex_html += "<body>\r\n<h1>Index /</h1>\r\n<hr>\r\n<pre>\r\n";

    for (std::size_t i = 0; i < file_list.size(); ++i)
    {
        std::string name = file_list[i].first;
        int type = file_list[i].second;
        if (type == DT_DIR)
            name += "/";
        autoindex_html += "<a href=\"" + name + "\">" + name + "</a>\r\n";
    }
    autoindex_html += "</pre>\r\n<hr>\r\n</body>\r\n</html>";
    std::cout << "-------------AUTOINDEX:\n" << autoindex_html << std::endl;
    return autoindex_html;
}

bool Response::create_autoindex_page(const std::string &path_to_index)
{
    DIR * dir = opendir(path_to_index.c_str());
    if (!dir)
        return false;
    dirent *file = readdir(dir);

    std::vector<std::pair<std::string, int> > file_list;
    while(file != NULL)
    {
        std::string file_name = file->d_name;
        int file_type = file->d_type;
        file_list.push_back(std::make_pair(file_name, file_type));
        file = readdir(dir);
    }
    std::cout << "-------------LETSGOOOOOOOOOOOOOOOOOOOO\n" << std::endl;

    autoindex_ = create_page(file_list);
    return true;
}   