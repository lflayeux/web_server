#include "../include/Request.hpp"
#include "../include/CGI.hpp"
#include "../include/Response.hpp"
#include "../include/Config.hpp"

#include <algorithm>

int handle_request(const std::string &request, Request &our_request)
{
	std::string line;
	std::istringstream iss(request);

	/* 1. Parsing of the request line (METHOD) */
	if (std::getline(iss, line))
	{
		// we erase the char '\r' if it does exist in our first line
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		
		std::istringstream	line_stream(line);
		std::string			method, path, version;

		line_stream >> method >> path >> version;
		our_request.set_method(method);
		our_request.set_path(path);
		our_request.set_version(version);
	}

	while (std::getline(iss, line))
	{
		// we erase the char '\r' if it does exist in our first line
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.empty())
			break;
		std::cout << "<line> ["<< line << "]" << std::endl;

		// pour split "Header: Value"
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos)
        {
            std::string header_name = line.substr(0, colon_pos);
            std::string header_value = line.substr(colon_pos + 1);
            
            // Enlever les espaces au start de la value
            size_t start = header_value.find_first_not_of(" \t");
            if (start != std::string::npos)
                header_value = header_value.substr(start);
            
            // Stocker
            our_request.add_header(header_name, header_value);
            
            // Parser les headers importants
            if (header_name == "Host")
            {
                // "localhost:8080" -> extraire le port
                size_t port_pos = header_value.find(':');
                if (port_pos != std::string::npos)
                {
                    std::string port_str = header_value.substr(port_pos + 1);
					our_request.set_port(static_cast<int>(std::strtol(port_str.c_str(), NULL, 10)));
                }
            }
            else if (header_name == "Content-Length")
				our_request.set_content_length(header_value);
        }
	}
	// 3. PARSER LE BODY (tout ce qui reste)
	std::string body;
	std::string remaining;
	while (std::getline(iss, remaining))
	{
		body += remaining;
		if (iss.peek() != EOF)  // Pas la dernière ligne
			body += "\n";
	}
	our_request.add_body(body);
	std::cout << "Our request is : " << our_request << std::endl;
	return 0;
}

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << BRED "Use: ./serv [ficher .conf]" RESET << std::endl;
		return (1);
	}
	Response our_request;
	if (our_request.load(av[1]) != 0)
		return (1);

	std::map<int, std::string>	pending_requests;
	sockaddr_in client;// Port, type d'ad IP + ad IP
	
	
	std::vector<int>	all_ports = our_request.getPorts();
	std::vector<int>	server_socket_fds;
	// epoll
	int	epoll_fd = epoll_create1(EPOLL_CLOEXEC);// means CLOSE on exec
	
	try
	{
		server_socket_fds = create_multi_srv(all_ports, epoll_fd);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	epoll_event	srv_events_list[64];

	while (1)
	{
		int nb_events = 0;
		nb_events = epoll_wait(epoll_fd, srv_events_list, 64, -1);
		std::cout << BCYAN "NB OF EVENT" << nb_events <<  RESET << std::endl;
		for (int i = 0; i < nb_events; i++)
		{
			std::vector<int>::const_iterator fd_srv = std::find(server_socket_fds.begin(), server_socket_fds.end(), srv_events_list[i].data.fd);
			// If the event is on the server socket
			if (fd_srv != server_socket_fds.end())
			{
				socklen_t client_len = sizeof(client);

				int client_fd = accept(*fd_srv, (sockaddr*)&client, &client_len);
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
					std::string reponse = our_request.create_response();
					// std::string reponse = get_response("index.html");
					send(srv_events_list[i].data.fd, reponse.c_str(), reponse.size(), 0);
					// On a fini avec ce client
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, srv_events_list[i].data.fd, NULL);
					close(srv_events_list[i].data.fd);
					pending_requests.erase(srv_events_list[i].data.fd);
					std::cout << BRED "Client déconnecté" << RESET << std::endl;
				}
				else if (srv_events_list[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
					std::cerr << "\n\nQUIT ERROR\n\n";
			}
		}
	}
}
/* a faire : Parser la requete, faire différentes pages en html, */