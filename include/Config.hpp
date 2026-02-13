#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "colors.hpp"
# include <vector>
# include <string>
# include <map>
# include <iostream>
# include <fstream>
# include <sstream>
# include <istream>
# include <cstdlib>
# include <climits>
# include <cstring>
# include <sys/epoll.h>
# include <algorithm>
# include <csignal>

struct Location
{
	std::string					path; // le path de l'url matchant la requete (si pas error)
	std::string					root; // le dossier physique ou chercher les fichier (si pas prendre celui du serveur)
	long long					Max_body_size; // le body size max de la requete client (format : 1024 (octets), 12k ou K (Ko), Mo et Go egalement)
	std::vector<std::string>	method; // liste des methodes autorises 
	std::vector<std::string>	index; // fichier a chercher (ex: index.html parsing.html) si pas de match default = index.html et check autoindex
	bool						autoindex; //  si on et pas de .html trouver lsiting du genre ls
	int 						conf_error_code; // code d'erreur par defaut 200
	std::string					redirections; // lien de redirections si return code dans les 300
	bool						upload_allowed; // si off pas d'upload (off par defaut)
	std::string					upload_location; // si upload on obligatoire
	std::string					cgi_path_;
};


struct Server
{
	std::vector<int>			port; // port(s) d'ecoute du serveur pour ce site (pas de port => conf error)
	std::string					root; // le dossier physique ou chercher les fichier (si pas error)
	long long					Max_body_size; // le body size max de la requete client (format : 1024 (octets), 12k ou K (Ko), Mo et Go egalement)
	std::vector<Location>		location; // les differentes location des pages et leurs caracteristiques 
	std::map<int, std::string>	error_pages; // les pages d'error personnalises (si 0 utilises generiqeus)
	std::string					cgi_path_;
};

class Config
{
	private:
		std::vector<Server> server_; // Liste des sites (dans le .conf chaque server{} definit un site) 1 minimum sinon error
		// int	nb_serv_;
		std::string	script_name_;
		std::vector<std::string> cgi_extensions_;
	public:
		Config(){};
		virtual ~Config(){};
		
		std::vector<int>			getPorts()
		{
			std::vector<int>		all_ports;
			for (size_t i = 0; i < server_.size(); i++)
				all_ports.insert(all_ports.begin(), server_[i].port.begin(), server_[i].port.end());
			return (all_ports);
		};
		int		load(char *file_path);
		bool	parseMain(std::vector<std::string> tokens, size_t i);
		bool	parseServer(std::vector<std::string> tokens, size_t &start);
		bool	parseLocation(std::vector<std::string> tokens, size_t &start, Server &server);

		int						getIdServer(int port) const;
		
		int						getBestPath(std::string path, int server_id) const;
		std::string					getRoot(std::string path, int server_id);
		long						getMaxBodySize(std::string path, int server_id);
		bool						isMethodAllowed(std::string path, int server_id, std::string method);
		std::vector<std::string>	getIndex(std::string path, int server_id);
		bool						getAutoIndex(std::string path, int server_id);
		int							getConfErrorCode(std::string path, int server_id);
		std::string					getRedirections(std::string path, int server_id);
		bool						getUploadAllowed(std::string path, int server_id);
		std::string					getUploadLocation(std::string path, int server_id);
		void						setCgiExtensions();
		std::string					getErrorPage(int error, int server_id);

		bool						checkListen();
		bool						checkUpload();
		bool						checkRedir();

		bool						checkConfig();

		std::string					get_cgi_path(std::string path, int server_id) const;
		std::string					get_script_name() const
		{
			return script_name_;
		};
		std::vector<std::string>	get_cgi_extensions() const
		{
			return cgi_extensions_;
		};		
};

class Request;
class Response;

int 				parse_request(const std::string &request, Request &our_request);
std::vector<int>	create_multi_srv(const std::vector<int> &all_ports, const int &epoll_fd);
void				client_send_request(const epoll_event *srv_events_list, const int &i, std::map<int, std::string> &pending_request, const int &epoll_fd);
void				client_get_response(const epoll_event *srv_events_list, const int &i, std::map<int, std::string> &pending_requests, const int &epoll_fd, Response &our_request);
#endif