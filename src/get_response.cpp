#include <fstream>
# include "../include/Request.hpp"
# include "../include/Response.hpp"

// int Response::response_code_GET()
// {
//     if(!file_exist(get_path_to_send()))
// 		  return (404);
//     return (200);
// }

void Response::file_behavior()
{
	std::string path_to_find = get_path_to_send();
	// std::cout << BRED "------------------- PATH TO FIND : " << path_to_find << std::endl;
	std::string root_dir  = getRoot(path_to_find, getIdServer(getHostName(), get_port()));
	std::string full_path  = root_dir + path_to_find;
	if (!file_exist(full_path))
		return (set_response_code_message(404));
	set_path(full_path);
	set_response_code_message(200);
}


void Response::directory_behavior()
{
	std::string path_to_find = get_path_to_send();
	if (path_to_find[path_to_find.length() - 1] != '/')
	{
		set_response_code_message(301);
		return ;
	}
	std::string root_dir  = getRoot(path_to_find, getIdServer(getHostName(),get_port()));
	std::string full_path  = root_dir + path_to_find;
	std::string default_path = full_path + "index.html";
	std::vector<std::string> file_indexes = getIndex(path_to_find, getIdServer(getHostName(),get_port()));
	if (!file_indexes.empty())
	{
		for (std::vector<std::string>::iterator it = file_indexes.begin(); it != file_indexes.end(); ++it)
		{
			std::string file_path = full_path + *it;
			if(file_exist(file_path))
			{
				set_response_code_message(200);
				return (set_path(file_path));
			}
		}
	}
	// std::cout << BRED "------------------- TEST" << std::endl;
	if(file_exist(default_path))
	{
		set_response_code_message(200);
		return (set_path(default_path));
	}
	else if (getAutoIndex(path_to_find, getIdServer(getHostName(),get_port())) == true)
	{
		std::cout << BRED "------------------- PATH TO FIND X : " << full_path << std::endl;
		if(create_autoindex_page(full_path) == false)
			set_response_code_message(404);
		else		
			set_response_code_message(200);
	}
	else
		set_response_code_message(403);
}

void Response::response_GET()
{
	std::string path_to_find = get_path_to_send();
	if (path_to_find.find('.') == path_to_find.npos)
	{
		
		directory_behavior();
		std::cout << BRED "------------------- PATH TO PRINT DIR : " << get_path_to_send() << std::endl;
	}
	else
	{
		file_behavior();
		std::cout << BRED "------------------- PATH TO PRINT FILE : " << get_path_to_send() << std::endl;
	}
	// NEED TO CHECK AUTOINDEX AFTERWARDS
}
