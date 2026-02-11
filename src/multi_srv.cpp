#include "../include/Request.hpp"
#include "../include/CGI.hpp"
#include "../include/Response.hpp"
#include "../include/Config.hpp"

#include <algorithm>

std::vector<int>	create_multi_srv(const std::vector<int> &all_ports, const int &epoll_fd)
{
	std::vector<int>	server_socket_fds;
	sockaddr_in			srv;
	for (size_t i = 0; i < all_ports.size(); i++)
	{
		server_socket_fds.push_back(socket(AF_INET, SOCK_STREAM, 0));
		if (server_socket_fds.back() < 0)
		{
			std::cerr << "Error creating socket\n";
			throw std::exception();
		}
		int opt = 1;
		setsockopt(server_socket_fds.back(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		// Non blocking server
		int flags = fcntl(server_socket_fds.back(), F_GETFL);
		fcntl(server_socket_fds.back(), F_SETFL, flags | O_NONBLOCK);
		// memset(&srv, 0, sizeof(srv));
		srv.sin_family = AF_INET;
		srv.sin_addr.s_addr = INADDR_ANY;
		srv.sin_port = htons(all_ports[i]);
		if (bind(server_socket_fds.back(), (sockaddr*)&srv, sizeof(srv)) < 0)
		{
			std::cerr << "Error binding socket to IP/port\n";
			throw std::exception();
		}
		
		if (listen(server_socket_fds.back(), SOMAXCONN) < 0)
		{
			std::cerr << "Error listening socket\n";
			throw std::exception();
		}
		else
		std::cout << "This socket has the ability to queue for incoming connexions (TCP)\n";
		std::cout << "Server listening on port " << all_ports[i] << "...\n";
		
		epoll_event	srv_events;
		srv_events.data.fd = server_socket_fds.back();
		srv_events.events = EPOLLIN;
		
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket_fds.back(), &srv_events);
	}
	return (server_socket_fds);
}