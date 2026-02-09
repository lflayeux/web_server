#include "../include/Config.hpp"

// GETTER/SETTER


/** **/
int Config::getIdServer(int port)
{
	for (size_t i = 0; i < server_.size(); i++)
	{
		for (size_t j = 0; j < server_[i].port.size(); j++)
		{
			if (port == server_[i].port[j])
				return (i);
		}
	}
	return (-1);
}

int		Config::getBestPath(std::string path, int server_id)
{
	int id = -1;
    size_t longest_match = 0;

    for (size_t i = 0; i < server_[server_id].location.size(); i++)
    {
        const std::string& loc_path = server_[server_id].location[i].path;
		if (path.find(loc_path) == 0)
        {
            if (loc_path.size() > longest_match)
            {
                longest_match = loc_path.size();
                id = i;
            }
        }
    }
    return (id);
}

std::string Config::getRoot(std::string path, int server_id)
{
	std::string best_root = server_[server_id].root;
	int i = getBestPath(path, server_id); 
	if (i != -1)
		best_root = server_[server_id].location[i].root;
    return (best_root);
}

std::string Config::getErrorPage(int error, int server_id)
{
	std::map<int, std::string>::iterator it = server_[server_id].error_pages.find(error);
	if (it != server_[server_id].error_pages.end())
		return (it->second);
	return ("default");
}
bool	Config::getUploadAllowed(std::string path, int server_id)
{
	int i = getBestPath(path, server_id);
	if (i == -1)
		return (false);
	return (server_[server_id].location[i].upload_allowed);
}

std::string	Config::getUploadLocation(std::string path, int server_id)
{
	int i = getBestPath(path, server_id);
	if (i != -1)
	{
		if (server_[server_id].location[i].upload_location != "default")
			return (server_[server_id].location[i].upload_location);
	}
	return ("./data/uploads");
}

bool	Config::getAutoIndex(std::string path, int server_id)
{
	int i = getBestPath(path, server_id);
	if (i == -1)
		return (false);
	return (server_[server_id].location[i].autoindex);
}

std::vector<std::string> Config::getIndex(std::string path, int server_id)
{
	std::vector<std::string> empty;
	int i = getBestPath(path, server_id);
	if (i == -1)
		return (empty);
	return (server_[server_id].location[i].index);
}
bool Config::isMethodAllowed(std::string path, int server_id, std::string method)
{
	std::vector<std::string> empty;
	int i = getBestPath(path, server_id);
	if (i != -1)
	{
		for (size_t n = 0; n < server_[server_id].location[i].method.size(); n++)
		{

			if (server_[server_id].location[i].method[n] == method)
				return (true);
		}
		return (false);
	}
	return (true);
}

int	Config::getConfErrorCode(std::string path, int server_id)
{
	int i = getBestPath(path, server_id);
	if (i != -1)
		return (server_[server_id].location[i].conf_error_code);
	return (200);
}
std::string	Config::getRedirections(std::string path, int server_id)
{
	int i = getBestPath(path, server_id);
	return (server_[server_id].location[i].redirections);
}

long	Config::getMaxBodySize(std::string path, int server_id)
{
	int i = getBestPath(path, server_id);
	if (i != -1)
	{
		if (server_[server_id].location[i].Max_body_size != -1)
			return (server_[server_id].location[i].Max_body_size);
	}
	return (server_[server_id].Max_body_size);
}
// =============================================================
// ===================== PARSING AND LOAD OBJECT ===============
// =============================================================

void removeComments(std::string& content) {
    size_t pos = content.find('#');
    
    while (pos != std::string::npos) {
        // On cherche la fin de la ligne actuelle
        size_t endLine = content.find('\n', pos);
        
        if (endLine == std::string::npos) {
            // Si pas de \n, c'est que le commentaire est à la toute fin du fichier
            content.erase(pos);
            break;
        } else {
            // On efface du '#' jusqu'au '\n' (on garde le \n pour ne pas coller 2 lignes)
            content.erase(pos, endLine - pos);
        }
        // On cherche le prochain '#'
        pos = content.find('#');
    }
}

void sanitize(std::string &s)
{
    std::string result;
    for (size_t i = 0; i < s.length(); ++i)
	{
        if (s[i] == '{' || s[i] == '}' || s[i] == ';')
		{
            result += " ";
            result += s[i];
            result += " ";
        }
		else
            result += s[i];
    }
    s = result;
}

std::vector<std::string>	tokenize(std::string input)
{
	std::vector<std::string> tokens;
	std::stringstream content;
	std::string token;

	content << input;
	while (content >> token)
		tokens.push_back(token);

	// for (size_t i = 0; i < tokens.size(); i++)
	// 	std::cout << tokens[i] << std::endl;
	return tokens;
}

bool	checkBraces(std::vector<std::string> tokens)
{
	int	count = 0;
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i] == "{")
			count++;
		else if (tokens[i] == "}")
			count--;
		if (count < 0)
			return (false);
	}
	if (count != 0)
		return (false);
	return (true);
}
bool isServer(std::vector<std::string> tokens, size_t &i)
{
	if (tokens[i] != "server")
		return (false);
	i++;
	if ((i > tokens.size()) || tokens[i] != "{")
		return (false);
	i++;
	if ((i > tokens.size()))
		return (false);
	return (true);
}

bool	parseRoot(std::vector<std::string> tokens, size_t &i, Location &location)
{
	i++;
	if (i < tokens.size())
	{
		location.root = tokens[i];
		i++;
	}
	else
		return (false);
	if (i >= tokens.size() || tokens[i] != ";")
		return (false);
	return (true);
}

bool	parseMethod(std::vector<std::string> tokens, size_t &i, Location &location)
{
	i++;
	while (i < tokens.size() && tokens[i] != ";")
	{
		if (!(tokens[i] == "GET" || tokens[i] == "DELETE" || tokens[i] == "POST"))
			return (false);
		location.method.push_back(tokens[i]);
		i++;
	}
	if (i >= tokens.size() || tokens[i] != ";")
		return (false);
	return (true);
}

bool	parseIndex(std::vector<std::string> tokens, size_t &i, Location &location)
{
	i++;
	while (i < tokens.size() && tokens[i] != ";")
	{
		location.index.push_back(tokens[i]);
		i++;
	}
	if (i >= tokens.size() || tokens[i] != ";")
		return (false);
	return (true);
}

bool	parseAutoIndex(std::vector<std::string> tokens, size_t &i, Location &location)
{
	i++;
	if (!(i < tokens.size()))
		return (false);
	if (tokens[i] == "on")
	{
		location.autoindex = true;
	}
	else if (tokens[i] == "off")
		location.autoindex = false;
	else
		return (false);
	i++;
	if (i >= tokens.size() || tokens[i] != ";")
		return (false);
	return (true);
}

bool	parseRedirections(std::vector<std::string> tokens, size_t &i, Location &location)
{
	i++;
	if (!(i < tokens.size()))
		return (false);

	if (i + 1 < tokens.size() && tokens[i + 1] == ";")// cas 1 seul arg
	{
		if (atoi(tokens[i].c_str()))// on a un code
		{
			if (atoi(tokens[i].c_str()) >= 200 && atoi(tokens[i].c_str()) <= 599)
				location.conf_error_code = atoi(tokens[i].c_str());
			else
				return (false);
		}
		else
			location.redirections = tokens[i];
		i++;
	}
	else if (i + 2 < tokens.size() && tokens[i + 2] == ";")// cas 2 args
	{
		if (atoi(tokens[i].c_str()))// on a un code en premier arg
		{
			if (atoi(tokens[i].c_str()) >= 200 && atoi(tokens[i].c_str()) <= 599)
				location.conf_error_code = atoi(tokens[i].c_str());
			else
				return (false);
		}
		else
			return (false);
		location.redirections = tokens[i + 1];// on charge le 2e arg
		i += 2;
	}
	else
		return (false);
	return (true);
}

bool	parseUploadAllowed(std::vector<std::string> tokens, size_t &i, Location &location)
{
	i++;
	if (!(i < tokens.size()))
		return (false);
	if (tokens[i] == "on")
		location.upload_allowed = true;
	else if (tokens[i] == "off")
		location.upload_allowed = false;
	else
		return (false);
	i++;
	if (i >= tokens.size() || tokens[i] != ";")
		return (false);
	return (true);
}

bool	parseUploadLocation(std::vector<std::string> tokens, size_t &i, Location &location)
{
	i++;
	if (i < tokens.size())
		location.upload_location = tokens[i];
	else
		return (false);
	i++;
	if (i >= tokens.size() || tokens[i] != ";")
		return (false);
	return (true);
}
bool	parseClientMaxBodySize(std::vector<std::string> tokens, size_t &i, Location &location)
{
	i++;
	std::string input = tokens[i].c_str();
	char end = input[input.size() - 1];
	long nb = 0;
	long multiplier = 1;
	if (isalpha(end))
	{
		end = toupper(end);
		if (end == 'K')
			multiplier = 1024;
		if (end == 'M')
			multiplier = 1024 * 1024;
		if (end == 'G')
			multiplier = 1024 * 1024 * 1024;
		input.erase(input.size() - 1);
		for (size_t j = 0; j < input.size(); ++j)
		{
    		if (!isdigit(input[j]))
        		return false;
		}
		nb = atoi(input.c_str());
		if (nb < 1 || nb > LONG_MAX)
			return (false);
	}
	else
	{
		nb = atoi(tokens[i].c_str());
		if (nb < 1 || nb > LONG_MAX)
			return (false);
	}
	nb *= multiplier;
	if (nb < 1 || nb > LONG_MAX)
		return (false);
	location.Max_body_size = nb;
	i++;
	if (i >= tokens.size() || tokens[i] != ";")
		return (false);
	return (true);
}
void	setDefaultLocation(Location &location)
{

	// std::string				path; // le path de l'url matchant la requete (si pas error)
	// std::string				root; // le dossier physique ou chercher les fichier (si pas prendre celui du serveur)
	// long						Max_body_size; // le body size max de la requete client (format : 1024 (octets), 12k ou K (Ko), Mo et Go egalement)
	// bool						autoindex; //  si on et pas de .html trouver lsiting du genre ls
	// int 						conf_error_code; // code d'erreur par defaut 200
	// std::string				redirections; // lien de redirections si return code dans les 300
	// bool						upload_allowed; // si off pas d'upload (off par defaut)
	// std::string				upload_location; 	
	location.path = "none";
	location.root = "none";
	location.Max_body_size = -1;
	location.autoindex = false;
	location.conf_error_code = 200;
	location.redirections = "none";
	location.upload_allowed = false;
	location.upload_location = "default";
}
bool	Config::parseLocation(std::vector<std::string> tokens, size_t &start, Server &server)
{
	Location	location;
	setDefaultLocation(location);
	if (!(start < tokens.size() && tokens[start][0] == '/'))
		return(false);
	location.path = tokens[start];
	start++;
	if (!(start < tokens.size() && tokens[start] == "{"))
		return (false);
	start++;
	size_t i = start;
	while (i < tokens.size())
	{
		if (tokens[i] == "root")
		{
			if (!parseRoot(tokens, i, location))
				return (false);
		}
		else if (tokens[i] == "allowed_method")
		{
			if (!parseMethod(tokens, i, location))
				return (false);
		}
		else if (tokens[i] == "index")
		{
			if (!parseIndex(tokens, i, location))
				return (false);
		}
		else if (tokens[i] == "autoindex")
		{
			if (!parseAutoIndex(tokens, i, location))
				return (false);
		}
		else if (tokens[i] == "return")
		{
			if (!parseRedirections(tokens, i, location))
				return (false);
		}
		else if (tokens[i] == "upload_allowed")
		{
			if (!parseUploadAllowed(tokens, i, location))
				return (false);
		}
		else if (tokens[i] == "upload_location")
		{
			if (!parseUploadLocation(tokens, i, location))
				return (false);
		}
		else if (tokens[i] == "client_max_body_size")
		{
			if (!parseClientMaxBodySize(tokens, i, location))
				return (false);
		}
		else if (tokens[i] == "}")
			break;
		else
			return (false);
		i++;
	}
	start = i;
	server.location.push_back(location);
	return (true);
}

void	setDefaultServer(Server &server)
{
	server.root = "none";
	server.Max_body_size = 10000;
}

bool	parseListen(std::vector<std::string> tokens, size_t &i, Server &server)
{
	i++;
	if (!(i < tokens.size()))
		return (false);
	long new_port = atoi(tokens[i].c_str());
	if (new_port < 1 || new_port > 65535)
		return (false);
	server.port.push_back(new_port);
	i++;
	if (i >= tokens.size() || tokens[i] != ";")
		return (false);
	return (true);
}

bool	parseClientMaxBodySize(std::vector<std::string> tokens, size_t &i, Server &server)
{
	i++;
	std::string input = tokens[i].c_str();
	char end = input[input.size() - 1];
	long nb = 0;
	long multiplier = 1;
	if (isalpha(end))
	{
		end = toupper(end);
		if (end == 'K')
			multiplier = 1024;
		if (end == 'M')
			multiplier = 1024 * 1024;
		if (end == 'G')
			multiplier = 1024 * 1024 * 1024;
		input.erase(input.size() - 1);
		for (size_t j = 0; j < input.size(); ++j)
		{
    		if (!isdigit(input[j]))
        		return false;
		}
		nb = atoi(input.c_str());
		if (nb < 1 || nb > LONG_MAX)
			return (false);
	}
	else
	{
		nb = atoi(tokens[i].c_str());
		if (nb < 1 || nb > LONG_MAX)
			return (false);
	}
	if (nb < 1 || nb > LONG_MAX)
			return (false);
	nb *= multiplier;
	server.Max_body_size = nb;
	i++;
	if (i >= tokens.size() || tokens[i] != ";")
		return (false);
	return (true);
}

bool	parseRoot(std::vector<std::string> tokens, size_t &i, Server &server)
{
	i++;
	if (i < tokens.size())
	{
		server.root = tokens[i];
		i++;
	}
	else
		return (false);
	if (i >= tokens.size() || tokens[i] != ";")
		return (false);
	return (true);
}
bool	parseErrorPage(std::vector<std::string> tokens, size_t &i, Server &server)
{
    i++;
    std::vector<std::string> args;
    while (i < tokens.size() && tokens[i] != ";")
	{
        args.push_back(tokens[i]);
        i++;
    }
    if (args.size() < 2 || (i >= tokens.size() || tokens[i] != ";"))
        return (false);
    std::string path = args.back();
    args.pop_back();
    for (size_t j = 0; j < args.size(); ++j)
	{
        for (size_t k = 0; k < args[j].size(); k++)
		{
            if (!isdigit(args[j][k]))
				return false;
        }
        int code = atoi(args[j].c_str());
        if (code < 300 || code > 599)
			return false;
        server.error_pages[code] = path;
    }
    return true;
}

bool	Config::parseServer(std::vector<std::string> tokens, size_t &i)
{
	Server server;
	setDefaultServer(server);
	while (i < tokens.size())
	{
		if (tokens[i] == "location")
		{
			i++;
			if (!parseLocation(tokens, i, server))
				return (false);

		}
		else if (tokens[i] == "root")
		{
			if (!parseRoot(tokens, i, server))
				return (false);
		}
		else if (tokens[i] == "listen")
		{
			if (!parseListen(tokens, i, server))
				return (false);
		}
		else if (tokens[i] == "client_max_body_size")
		{
			if (!parseClientMaxBodySize(tokens, i, server))
				return (false);
		}
		else if (tokens[i] == "error_page")
		{
			if (!parseErrorPage(tokens, i, server))
				return (false);
		}
		else if (tokens[i] == "}")
		{
			i++;
			break;
		}
		else
			return (false);
		i++;
	}
	server_.push_back(server);
	return (true);
}


bool Config::parseMain(std::vector<std::string> tokens, size_t i)
{
	while (i < tokens.size())
	{
		if (i > tokens.size())
			return (false);
		if (!isServer(tokens, i)) // is_server
			return (false);
		if (!parseServer(tokens, i))
			return (false);
	}
	return (true);
}


int Config::load(char *file_path)
{
	/* verif que le fichier existe bien et qu'on peut l'ouvrir */
	std::ifstream file(file_path);
	if (!file.is_open())
	{
		std::cerr << BRED "Error: can't open file" RESET << std::endl;
		return -1;
	}
	std::stringstream content;
	std::string			tmp;

	content << file.rdbuf();
	tmp = content.str();
	sanitize(tmp);
	removeComments(tmp);
	// std::cout << content.str() << std::endl;

	std::vector<std::string> tokens = tokenize(tmp);
	if (!checkBraces(tokens))
	{
		std::cerr << BRED "Error: wrong braces\n" RESET;
		return (-1);
	}
	if (!parseMain(tokens, 0))
	{
		std::cerr << BRED "Error: .conf not valid" RESET << std::endl;
		return (-1);
	}
	// ======================
	// = PRINT SERVERS TEST =
	// ======================
	// for (size_t i = 0; i < server_.size(); i++)
	// {

	// 	std::cout << BMAGENTA "SERVER "  << i + 1 <<  RESET << std::endl;
	// 	for (size_t n = 0; n < server_[i].port.size(); n++)
	// 	{
	// 		std::cout << "PORT ==> " << server_[i].port[n] << std::endl;
	// 	}
	// 	std::cout << "MAX_BODY_SIZE ==> " << server_[i].Max_body_size << std::endl;
	// 	std::cout << "ROOT ==> " << server_[i].root << std::endl;

	// 	std::cout << BMAGENTA "ERROR PAGES:" RESET << std::endl;
	// 	for (std::map<int, std::string>::iterator it = server_[i].error_pages.begin(); it != server_[i].error_pages.end(); ++it) 
	// 	{
	// 	    std::cout << "  Code [" << it->first << "] => Path: " << it->second << std::endl;
	// 	}
	// 	for (size_t j = 0; j < server_[i].location.size(); j++)
	// 	{
	// 		std::cout << BCYAN "LOCATIONS "  << j + 1 <<  RESET << std::endl;

	// 		std::cout << "PATH ==> " << server_[i].location[j].path << std::endl;
	// 		std::cout << "ROOT ==> " << server_[i].location[j].root << std::endl;
	// 		std::cout << "AUTOINDEX ==> " << server_[i].location[j].autoindex << std::endl;
	// 		std::cout << "ERROR_CODE ==> " << server_[i].location[j].conf_error_code << std::endl;
	// 		std::cout << "REDIRECTIONS ==> " << server_[i].location[j].redirections << std::endl;
	// 		std::cout << "UPLOAD_ALLOWED ==> " << server_[i].location[j].upload_allowed << std::endl;
	// 		std::cout << "UPLOAD_LOCATIONS ==> " << server_[i].location[j].upload_location << std::endl;
	// 		std::cout << "MAX_BODY_SIZE ==> " << server_[i].location[j].Max_body_size << std::endl;
	// 	}
	// }

	// std::cout << std::endl;
	// std::cout << std::endl;
	// std::cout << "Best root: " << getRoot("/size", 0) << std::endl;
	// std::cout << "Server Id: " << getIdServer(3030) << std::endl;
	// std::cout << "MaxBODYSIZE: " << getMaxBodySize("/size", 0) << std::endl;
	// std::cout << "Server Method: " << isMethodAllowed("/", 0, "GET") << std::endl;
	// std::cout << "Server Index: ";
	// std::vector<std::string> indexs = getIndex("/uploads", 0);
	// for (size_t n = 0; n < indexs.size(); n++)
	// {
	// 	std::cout << " " << indexs[n];
	// }
	// std::cout << std::endl;
	// std::cout << "Server AutoIndex: " << getAutoIndex("/", 0) << std::endl;
	// std::cout << "Server Conf_error_code: " << getConfErrorCode("/uploads/lolololo", 0) << std::endl;


	// std::cout << "Server Upload Allowed: " << getUploadAllowed("/size", 0) << std::endl;
	// std::cout << "Server Upload Location: " << getUploadLocation("/uploads", 0) << std::endl;
	// std::cout << "Server Error_page: " << getErrorPage(200, 0) << std::endl;

	return 0;
}
// PARSE_MAIN

// DES QUE SERVER + { => PARSE_SERVER => }
//LOCATION + PATH + { => PARSE_LOCATION => }