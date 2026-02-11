#include "../include/Request.hpp"
#include "../include/CGI.hpp"
#include "../include/Response.hpp"
#include "../include/Config.hpp"

#include <algorithm>

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
					client_send_request(srv_events_list, i, pending_requests, epoll_fd);
				// ETAPE 2 = on peut maintenant envoyer la reponse
				else if (srv_events_list[i].events & EPOLLOUT)
					client_get_response(srv_events_list, i, pending_requests, epoll_fd, our_request);
				else if (srv_events_list[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
					std::cerr << "\n\nQUIT ERROR\n\n";
			}
		}
	}
}
/* a faire : Parser la requete, faire différentes pages en html, */