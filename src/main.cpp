#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include "../include/webserv.hpp"

int	main(int ac, char **av)
{
	(void)ac;
	(void)av;
	sockaddr_in	srv, client;

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
		std::cout << "NB OF EVENT" << nb_events << std::endl;
		for (int i = 0; i < nb_events; i++)
		{

			// If the event is on the server socket
			if (srv_events_list[i].data.fd == fd_srv)
			{
				socklen_t client_len = sizeof(client);

				int client_fd = accept(fd_srv, (sockaddr*)&client, &client_len);
				std::cout << "New client connected: fd=" << client_fd << "\n";
				int flags = fcntl(client_fd, F_GETFL);
				fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
				// Add client_fd to epoll (important!)
				epoll_event ev;
				ev.events = EPOLLIN;      // listen for read events
				ev.data.fd = client_fd;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0)
					std::cerr << "Failed to add client fd to epoll\n";
			}
			else
			{
				char buffer[4000];
				ssize_t bytes = 0;
				std::string full_data = "";
				while ((bytes = recv(srv_events_list[i].data.fd, buffer, sizeof(buffer) - 1, 0)) > 0)
					full_data.append(buffer);
				// const char* msg = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, world!";
				// send(srv_events_list[i].data.fd, msg, strlen(msg), 0);
				std::cout << full_data << std::endl;
				const char *msg = get_response("index.html").c_str();
				send(srv_events_list[i].data.fd, msg, strlen(msg), 0);
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, srv_events_list[i].data.fd, NULL);
				close(srv_events_list[i].data.fd);
				// close(fd_srv);
			}
		}
	}
}