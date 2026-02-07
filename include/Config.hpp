#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <vector>
# include <string>
# include <map>
# include <iostream>
# include <fstream>
# include "colors.hpp"
# include <sstream>
# include <istream>

struct Location
{
	std::string					path; // le path de l'url matchant la requete (si pas error)
	std::string					root; // le dossier physique ou chercher les fichier (si pas prendre celui du serveur)
	std::vector<std::string>	method; // liste des methodes autorises 
	std::vector<std::string>	index; // fichier a chercher (ex: index.html parsing.html) si pas de match default = index.html et check autoindex
	bool						autoindex; //  si on et pas de .html trouver lsiting du genre ls
	int 						error_code; // code d'erreur par defaut 200
	std::string					redirections; // lien de redirections si return code dans les 300
	bool						upload_allowed; // si off pas d'upload (off par defaut)
	std::string					upload_location; // si upload on obligatoire
};


struct Server
{
	std::vector<int>			port; // port(s) d'ecoute du serveur pour ce site (pas de port => conf error)
	std::string					root; // le dossier physique ou chercher les fichier (si pas error)
	std::string 				Max_body_size; // le body size max de la requete client (format : 1024 (octets), 12k ou K (Ko), Mo et Go egalement)
	std::vector<Location>		location; // les differentes location des pages et leurs caracteristiques 
	std::map<int, std::string>	error_pages; // les pages d'error personnalises (si 0 utilises generiqeus)
};

class Config
{
	private:
		std::vector<Server> server; // Liste des sites (dans le .conf chaque server{} definit un site) 1 minimum sinon error
		
	public:
		Config(/* args */){};
		~Config(){};
		

		int load(char *file_path);
		

};

// ETRE HYPER RIGOUREUX AVEC LE PARSING DES QUE TOKENS MANQUANT ERROR

// PARSE EN TOKEN CHAQUE LIGNE
//

#endif