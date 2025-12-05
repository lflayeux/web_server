#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

int	main(int ac, char **av)
{
	sockaddr_in	srv, client;
	int			client_list[10];

	int	fd_srv = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_srv < 0)
	{
		std::cerr << "Error creating socket\n";
		return (-1);
	}

	// Non blocking server
	fcntl(fd_srv, F_SETFL, O_NONBLOCK);
	// memset(&srv, 0, sizeof(srv));
	srv.sin_family = AF_INET;
	srv.sin_addr.s_addr = INADDR_ANY;
	srv.sin_port = htons(7070);
	
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
	std::cout << "Server listening on port 7070...\n";

	// epoll
	int	epoll_fd = epoll_create1(EPOLL_CLOEXEC);// means CLOSE on exec
	
	epoll_event	srv_events, srv_events_list[64];
	srv_events.events = EPOLLIN;
	srv_events.data.fd = fd_srv;

	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_srv, &srv_events);

	while (1)
	{
		int nb_events = epoll_wait(epoll_fd, srv_events_list, 64, -1);
		for (int i = 0; i < nb_events; i++)
		{

			// If the event is on the server socket
			if (srv_events_list[i].data.fd == fd_srv)
			{
				socklen_t client_len = sizeof(client);

				int client_fd = accept(fd_srv, (sockaddr*)&client, &client_len);
				std::cout << "New client connected: fd=" << client_fd << "\n";

				// Add client_fd to epoll (important!)
				epoll_event ev;
				ev.events = EPOLLIN;      // listen for read events
				ev.data.fd = client_fd;

				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0)
					std::cerr << "Failed to add client fd to epoll\n";
			}
			else
			{
				// if the event is on the client socket
				char	buffer[257];

				ssize_t bytes = recv(srv_events_list[i].data.fd, buffer, sizeof(buffer) - 1, 0);
				if (bytes == -1)
				{
    				perror("recv");
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, srv_events_list[i].data.fd, nullptr);
                    close(srv_events_list[i].data.fd);
				}
				else
				{
					const char* msg = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, world!";
					send(srv_events_list[i].data.fd, msg, strlen(msg), 0);
					std::cerr << "oops\n";
				}
				buffer[bytes] = '\0';
				std::cout << buffer << std::endl;
			}
		}
	}
}