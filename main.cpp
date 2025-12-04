#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

int	main(int ac, char **av)
{
	sockaddr_in	srv, client;
	int			client_list[10];
	std::cerr << "coucou\n";

	int	fd_srv = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_srv < 0)
		return (-1);
	std::cerr << "coucou -1\n";

	memset(&srv, 0, sizeof(srv));
	srv.sin_family = AF_INET;
	srv.sin_addr.s_addr = INADDR_ANY;
	srv.sin_port = htons(8080);
	
	int	bind_srv = bind(fd_srv, (sockaddr*)&srv, sizeof(srv));
	if (bind_srv < 0)
		return (-2);

	int	listen_srv = listen(fd_srv, 5);
	if (listen_srv < 0)
		return (-3);
	
	std::cerr << "coucou -3\n";
	// epoll
	int	epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	
	epoll_event	srv_events, srv_events_list[2];

	srv_events.events = EPOLLIN;
	srv_events.data.fd = fd_srv;

	std::cerr << "coucou -4\n";


	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_srv, &srv_events);
	std::cerr << "coucou -5\n";

	std::cerr << "coucou -6\n";
	while (1)
	{
		int nb_events = epoll_wait(epoll_fd, srv_events_list, 1, -1);
		for (int i = 0; i < nb_events; i++)
		{

			// If the event is on the server socket
			if (srv_events_list[i].data.fd == fd_srv)
			{
				
				sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);

				int client_fd = accept(fd_srv, (sockaddr*)&client_addr, &client_len);
				send(client_fd, "coucou", 6, 0);
				std::cout << "New client connected: fd=" << client_fd << "\n";

				// Add client_fd to epoll (important!)
				epoll_event ev;
				ev.events = EPOLLIN;      // listen for read events
				ev.data.fd = client_fd;

				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0)
					std::cerr << "Failed to add client fd to epoll\n";
			}
		}
		nb_events = 0;
	}
}