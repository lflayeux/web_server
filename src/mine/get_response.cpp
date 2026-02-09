#include <fstream>
# include "../../include/webserv.hpp"
# include "../../include/Response.hpp"

int Response::response_code_GET()
{
    if(!file_exist(get_path_to_send()))
		return (404);
    return (200);
}

// void Response::create_path()
// {
// 	root_dir = getRoot(get_path_to_send().sub(getRoot().length() - 1), getIdServer(get_port()));
// 	path_to_find = get_path_to_send();
// 	std::vector<std::string> file_indexes = getIndex(get_path_to_send().sub(getRoot().length() - 1), getIdServer(get_port()));

// 	for (std::vector<std::string>::iterator it = file_indexes.begin(); it != file_indexes.end(); ++it)
// 	{
// 		DIR * dir = opendir(path_to_find.c_str());
// 		struct dirent *file = readdir(dir);
// 		while(file != NULL)
// 		{
// 			std::string file_name = file->d_name;
// 			struct stat st;
// 			stat(file_name, &st);
// 			if(S_ISREG(st.st_mode))
// 			{
// 				if(file_name == *it)
// 				{
// 					set_path(file_name);
// 					return ;
// 				}
// 			}
// 			file = readdir(dir);
// 		}
// 		closedir(path_to_find);
// 	}
// 	DIR * dir = opendir(path_to_find.c_str());
// 	struct dirent *file = readdir(dir);
// 	while(file != NULL)
// 	{
// 		std::string file_name = file->d_name;
// 		struct stat st;
// 		stat(file_name, &st);
// 		if(S_ISREG(st.st_mode))
// 		{
// 			if(file_name == "index.html")
// 			{
// 				set_path(file_name);
// 				return ;
// 			}
// 		}
// 		file = readdir(dir);
// 	}
// 	closedir(path_to_find);
// }

void Response::response_GET()
{
	set_response_code_message(response_code_GET());
}
