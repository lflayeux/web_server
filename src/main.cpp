#include "../include/webserv.hpp"


int handle_request(const std::string &request, http_request &our_request)
{
	/* PARSING */

	std::string line;
	std::istringstream iss(request);

	while (std::getline(iss, line))
	{
		std::cout << "+=+=+=+=: "<< line << std::endl;
		if (line.find("GET") != std::string::npos)
			our_request.set_method_Get(line);
		else if (line.find("POST") != std::string::npos)
			our_request.set_method(POST);
		else if (line.find("DELETE") != std::string::npos)
			our_request.set_method(DELETE);
		else if (line.find("8080") != std::string::npos)
			our_request.set_port(8080);
	}

	std::cout << "Our request is :" << our_request << std::endl;
	/* TTT */

	return 0;
}


int	main(int ac, char **av)
{
	(void)ac;
	(void)av;
	http_request	our_request;

	std::map<int, std::string>	pending_requests;
	sockaddr_in	srv, client;// Port, type d'ad IP + ad IP

	int	fd_srv = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_srv < 0)
	{
		std::cerr << "Error creating socket\n";
		return (-1);
	}
	int opt = 1;
	setsockopt(fd_srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// Non blocking server
	int flags = fcntl(fd_srv, F_GETFL);
	fcntl(fd_srv, F_SETFL, flags | O_NONBLOCK);
	// memset(&srv, 0, sizeof(srv));
	srv.sin_family = AF_INET;
	srv.sin_addr.s_addr = INADDR_ANY;
	srv.sin_port = htons(8080);
	
	if (bind(fd_srv, (sockaddr*)&srv, sizeof(srv)) < 0)
	{
		std::cerr << "Error binding socket to IP/port\n";
		return (-2);
	}

	if (listen(fd_srv, SOMAXCONN) < 0)
	{
		std::cerr << "Error listening socket\n";
		return (-3);
	}
	else
		std::cout << "This socket has the ability to queue for incoming connexions (TCP)\n";
	std::cout << "Server listening on port 8080...\n";

	// epoll
	int	epoll_fd = epoll_create1(EPOLL_CLOEXEC);// means CLOSE on exec
	
	epoll_event	srv_events, srv_events_list[64];
	srv_events.events = EPOLLIN;
	srv_events.data.fd = fd_srv;

	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_srv, &srv_events);

	while (1)
	{
		int nb_events = 0;
		nb_events = epoll_wait(epoll_fd, srv_events_list, 64, -1);
		std::cout << BCYAN "NB OF EVENT" << nb_events <<  RESET << std::endl;
		for (int i = 0; i < nb_events; i++)
		{

			// If the event is on the server socket
			if (srv_events_list[i].data.fd == fd_srv)
			{
				socklen_t client_len = sizeof(client);

				int client_fd = accept(fd_srv, (sockaddr*)&client, &client_len);
				std::cout << BMAGENTA "New client connected: fd=" << client_fd << RESET << "\n";
				int flags = fcntl(client_fd, F_GETFL);
				fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
				// Add client_fd to epoll (important!)
				epoll_event ev;
				ev.events = EPOLLIN;      // listen for read events
				ev.data.fd = client_fd;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0)
					std::cerr << "Failed to add client fd to epoll\n";
			}
			else	// client
			{
				// ETAPE 1 = le client evoie sa requete
				if (srv_events_list[i].events & EPOLLIN)	// le client envoie sa requete
				{
					char		buffer[4096];
					ssize_t		bytes = 0;
					std::string	full_data = "";
					while ((bytes = recv(srv_events_list[i].data.fd, buffer, sizeof(buffer) - 1, 0)) > 0)
					{
						buffer[bytes] = '\0';
						full_data.append(buffer);
					}
					if (full_data.find("\r\n\r\n") != std::string::npos)
					{
						std::cout << "Requête reçue :\n" << full_data << std::endl;
						// Il faudra stocker la requete quelque part
						// imaginons un container "requests" type map<int, std::string>
						pending_requests[srv_events_list[i].data.fd] = full_data;

						// maintenant qu'on a la requete, on veut écrire la réponse
						// on passe donc en mode EPOLLOUT
						epoll_event	ev;
						ev.events = EPOLLOUT;
						ev.data.fd = srv_events_list[i].data.fd;
						epoll_ctl(epoll_fd, EPOLL_CTL_MOD, srv_events_list[i].data.fd, &ev);
					}
				}

				// ETAPE 2 = on peut maintenant envoyer la reponse
				else if (srv_events_list[i].events & EPOLLOUT)
				{
					/* On a une string en arg, on veut la parser et la traiter */
					if (handle_request(pending_requests[srv_events_list[i].data.fd], our_request) != 0)
						std::cerr << "Error with handling request\n";// + envoyer code erreur
					std::cout << BMAGENTA "Envoi de la réponse..." << RESET << std::endl;
					// Générer la réponse (on devrait la stocker aussi dans un container)
					std::string reponse = get_response(our_request.get_path_to_send().c_str());
					// std::string reponse = get_response("index.html");
					send(srv_events_list[i].data.fd, reponse.c_str(), reponse.size(), 0);
					// On a fini avec ce client
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, srv_events_list[i].data.fd, NULL);
					close(srv_events_list[i].data.fd);
					pending_requests.erase(srv_events_list[i].data.fd);
					std::cout << BRED "Client déconnecté" << RESET << std::endl;
				}
				else if (srv_events_list[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
				{
					std::cerr << "\n\nQUIT ERROR\n\n";
				}
			}
		}
	}
}

/* a faire : Parser la requete, faire différentes pages en html, */