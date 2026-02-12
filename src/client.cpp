#include "../include/Request.hpp"
#include "../include/CGI.hpp"
#include "../include/Response.hpp"
#include "../include/Config.hpp"

void	client_send_request(const epoll_event *srv_events_list, const int &i, std::map<int, std::string> &pending_requests, const int &epoll_fd)
{
	char		buffer[4096];
	ssize_t		bytes = 0;
	std::string	full_data = "";
	while ((bytes = recv(srv_events_list[i].data.fd, buffer, sizeof(buffer) - 1, 0)) > 0)
	{
		buffer[bytes] = '\0';
		full_data.append(buffer);
	}
	// Si le client ferme la connexion ou envoie rien
	if (bytes == 0 || (bytes < 0 && errno != EAGAIN && errno != EWOULDBLOCK))
	{
		std::cout << BRED "Client disconnected (empty/closed)" << RESET << std::endl;
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, srv_events_list[i].data.fd, NULL);
		close(srv_events_list[i].data.fd);
		pending_requests.erase(srv_events_list[i].data.fd);
		return;
	}
	if (full_data.find("\r\n\r\n") != std::string::npos)
	{
		std::cout << "Requête reçue :\n" << full_data << std::endl;
		pending_requests[srv_events_list[i].data.fd] = full_data;
		// maintenant qu'on a la requete, on veut écrire la réponse
		// on passe donc en mode EPOLLOUT
		epoll_event	ev;
		ev.events = EPOLLOUT;
		ev.data.fd = srv_events_list[i].data.fd;
		epoll_ctl(epoll_fd, EPOLL_CTL_MOD, srv_events_list[i].data.fd, &ev);
	}
}

void	client_get_response(const epoll_event *srv_events_list, const int &i, std::map<int, std::string> &pending_requests, const int &epoll_fd, Response &our_request)
{
	/* On a une string en arg, on veut la parser et la traiter */
	if (parse_request(pending_requests[srv_events_list[i].data.fd], our_request) != 0)
		std::cerr << "Error with handling request\n";// + envoyer code erreur
	std::cout << BMAGENTA "Envoi de la réponse..." << RESET << std::endl;
	std::string	reponse;
	if (our_request.is_cgi_request())
	{
		std::cout << BGREEN << "Il s'agit d'un CGI\n" << RESET;
		CGI	my_cgi(our_request);
		try
		{
			reponse = my_cgi.execute();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
		
	}
	else
	{
		std::cout << BGREEN << "Il s'agit d'une requête standard\n" << RESET;
		reponse = our_request.create_response();
	}
	// Générer la réponse (on devrait la stocker aussi dans un container)
	// std::string reponse = get_response("index.html");
	send(srv_events_list[i].data.fd, reponse.c_str(), reponse.size(), 0);
	// On a fini avec ce client
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, srv_events_list[i].data.fd, NULL);
	close(srv_events_list[i].data.fd);
	pending_requests.erase(srv_events_list[i].data.fd);
	std::cout << BRED "Client déconnecté" << RESET << std::endl;
}