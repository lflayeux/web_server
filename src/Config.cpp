#include "../include/Config.hpp"

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

void sanitize(std::string &s) {
    std::string result;
    for (size_t i = 0; i < s.length(); ++i) {
        if (s[i] == '{' || s[i] == '}' || s[i] == ';') {
            result += " ";
            result += s[i];
            result += " ";
        } else {
            result += s[i];
        }
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
				location.error_code = atoi(tokens[i].c_str());
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
				location.error_code = atoi(tokens[i].c_str());
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
void	setDefaultLocation(Location &location)
{
	location.path = "none";
	location.root = "none";
	location.autoindex = false;
	location.error_code = 200;
	location.redirections = "none";
	location.upload_allowed = false;
	location.upload_location = "none";
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
		else if (tokens[i] == "}")
		{
			i++;
			break;
		}
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
	server.Max_body_size = "10000";
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
		else if (tokens[i] == "}")
		{
			i++;
			break;
		}
		else
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
		return (-1);
	}
	// PRINT SERVERS
	for (size_t i = 0; i < server_.size(); i++)
	{

		std::cout << BMAGENTA "SERVER "  << i + 1 <<  RESET << std::endl;
		for (size_t j = 0; j < server_[i].location.size(); j++)
		{
			std::cout << BCYAN "LOCATIONS "  << j + 1 <<  RESET << std::endl;

			std::cout << "PATH ==> " << server_[i].location[j].path << std::endl;
			std::cout << "ROOT ==> " << server_[i].location[j].root << std::endl;
			std::cout << "AUTOINDEX ==> " << server_[i].location[j].autoindex << std::endl;
			std::cout << "ERROR_CODE ==> " << server_[i].location[j].error_code << std::endl;
			std::cout << "REDIRECTIONS ==> " << server_[i].location[j].redirections << std::endl;
			std::cout << "UPLOAD_ALLOWED ==> " << server_[i].location[j].upload_allowed << std::endl;
			std::cout << "UPLOAD_LOCATIONS ==> " << server_[i].location[j].upload_location << std::endl;
		}
	}
	return 0;
}
// PARSE_MAIN

// DES QUE SERVER + { => PARSE_SERVER => }
//LOCATION + PATH + { => PARSE_LOCATION => }