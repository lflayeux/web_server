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

	for (size_t i = 0; i < tokens.size(); i++)
		std::cout << tokens[i] << std::endl;
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

int Config::load(char *file_path)
{
	// TEST
	std::cout << file_path << std::endl;

	// FUNCTION
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
	return 0;
}