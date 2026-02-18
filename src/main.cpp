#include "../include/Request.hpp"
#include "../include/CGI.hpp"
#include "../include/Response.hpp"
#include "../include/Config.hpp"

volatile sig_atomic_t server_running = 1;

void signal_handler(int signum)
{
    (void)signum;
    std::cerr << UMAGENTA << "\n[Signal] Interruption reçue. Fermeture propre..." << RESET << std::endl;
    server_running = 0;
}

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << BRED "Use: ./serv [ficher .conf]" RESET << std::endl;
		return (1);
	}
	// config du signal
	struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
	// end of config
	Response our_request;
	if (our_request.load(av[1]) != 0)
		return (1);

	std::map<int, std::string>	pending_requests;
	std::map<int, CGI *>		cgi_by_fd;
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
		std::cerr << BRED << "Impossible to create multi srv [check if not already launched]" << RESET << std::endl;
		return (1);
	}
	
	epoll_event	srv_events_list[64];
	while (server_running)
	{
		int nb_events = 0;
		nb_events = epoll_wait(epoll_fd, srv_events_list, 64, -1);
		for (int i = 0; i < nb_events; i++)
		{
			std::map<int, CGI *>::iterator cgi_it = cgi_by_fd.find(srv_events_list[i].data.fd);
			if (cgi_it != cgi_by_fd.end())
			{
				std::string cgi_response;
				if (cgi_it->second->read_output(cgi_response))
				{
					send(cgi_it->second->get_client_fd(), cgi_response.c_str(), cgi_response.size(), 0);
					close(cgi_it->second->get_client_fd());
					delete cgi_it->second;
					cgi_by_fd.erase(cgi_it);
				}
				continue;
			}
			std::vector<int>::const_iterator fd_srv = std::find(server_socket_fds.begin(), server_socket_fds.end(), srv_events_list[i].data.fd);
			// If the event is on the server socket
			if (fd_srv != server_socket_fds.end())
			{
				socklen_t client_len = sizeof(client);
				int client_fd = accept(*fd_srv, (sockaddr*)&client, &client_len);
				std::cout << BMAGENTA "New client connected: id[" << client_fd << "]" << RESET << "\n";
				int flags = fcntl(client_fd, F_GETFL);
				fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
				fcntl(client_fd, F_SETFD, FD_CLOEXEC);
				// Add client_fd to epoll (important!)
				epoll_event ev;
				ev.events = EPOLLIN | EPOLLET;      // listen for read events
				ev.data.fd = client_fd;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0)
					std::cerr << "Failed to add client fd to epoll\n";
			}
			else	// client
			{
				// ETAPE 1 = le client evoie sa requete
				if (srv_events_list[i].events & EPOLLIN)// le client envoie sa requete
					client_send_request(srv_events_list, i, pending_requests, epoll_fd);
				// ETAPE 2 = on peut maintenant envoyer la reponse
				else if (srv_events_list[i].events & EPOLLOUT)
					client_get_response(srv_events_list, i, pending_requests, cgi_by_fd, epoll_fd, our_request);
				else if (srv_events_list[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
					std::cerr << "\n\nQUIT ERROR\n\n";
			}
		}
	}
	for (size_t i = 0; i < server_socket_fds.size(); i++)
	{
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, server_socket_fds[i], NULL);
		close(server_socket_fds[i]);
	}
	close(epoll_fd);
	return (1);
}
/* a faire : Parser la requete, faire différentes pages en html, */