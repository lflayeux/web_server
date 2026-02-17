#include "../include/Request.hpp"
#include "../include/CGI.hpp"
#include "../include/Response.hpp"
#include "../include/Config.hpp"

void	client_send_request(const epoll_event *srv_events_list, const int &i, std::map<int, std::string> &pending_requests, const int &epoll_fd)
{
	// char		buffer[4096];
	// ssize_t		bytes = 0;
	// std::string	full_data = "";
	// while ((bytes = recv(srv_events_list[i].data.fd, buffer, sizeof(buffer) - 1, 0)) > 0)
	// {
	// 	full_data.append(buffer, bytes);
	// }
	// // Si le client ferme la connexion ou envoie rien
	// if (bytes == 0 || (bytes < 0 && full_data.empty()))
	// {
	// 	std::cout << BMAGENTA "Client disconnected (empty/closed)" << RESET << std::endl;
	// 	pending_requests.erase(srv_events_list[i].data.fd);
	// 	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, srv_events_list[i].data.fd, NULL);
	// 	close(srv_events_list[i].data.fd);
	// 	return;
	// }
	// // if (full_data.find("\r\n\r\n") != std::string::npos)
	// else
	// {
	// 	std::cout << BMAGENTA << "Request recieved :\n" << BYELLOW << full_data << RESET << std::endl;
	// 	pending_requests[srv_events_list[i].data.fd] = full_data;
	// 	// maintenant qu'on a la requete, on veut écrire la réponse
	// 	// on passe donc en mode EPOLLOUT | EPOLLET
	// 	epoll_event	ev;
	// 	ev.events = EPOLLOUT | EPOLLET;
	// 	ev.data.fd = srv_events_list[i].data.fd;
	// 	epoll_ctl(epoll_fd, EPOLL_CTL_MOD, srv_events_list[i].data.fd, &ev);
	// }

	char    buffer[8192]; 
    int     fd = srv_events_list[i].data.fd;
    ssize_t bytes;

    while ((bytes = recv(fd, buffer, sizeof(buffer), 0)) > 0)
    {
        pending_requests[fd].append(buffer, bytes);
    }
	if (bytes == 0 || (bytes < 0 && pending_requests[fd].empty()))
	{
		std::cout << BMAGENTA "Client disconnected (empty/closed)" << RESET << std::endl;
		pending_requests.erase(fd);
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
		return;
	}
    std::string &data = pending_requests[fd];
    size_t header_sep = data.find("\r\n\r\n");

    if (header_sep != std::string::npos)
    {
        size_t cl_pos = data.find("Content-Length: ");
        if (cl_pos != std::string::npos)
        {
            size_t start = cl_pos + 16;
            size_t end = data.find("\r\n", start);
            size_t content_length = std::atoll(data.substr(start, end - start).c_str());
            
            size_t total_received_body = data.size() - (header_sep + 4);

            if (total_received_body < content_length) {
                return; 
            }
        }
    }
    else
	{
        return;
    }
    epoll_event ev;
    ev.events = EPOLLOUT | EPOLLET;
    ev.data.fd = fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
}

void	client_get_response(const epoll_event *srv_events_list, const int &i, std::map<int, std::string> &pending_requests, std::map<int, CGI *> &cgi_by_fd, const int &epoll_fd, Response &our_request)
{
	/* On a une string en arg, on veut la parser et la traiter */
	if (parse_request(pending_requests[srv_events_list[i].data.fd], our_request) != 0)
		std::cerr << "Error with handling request\n";// + envoyer code erreur
	std::cout << BMAGENTA "Sending response..." << RESET << std::endl;
	std::string	reponse;
	std::string pathToFind = our_request.get_path_to_send();
	std::string tmp = our_request.getRoot(pathToFind, our_request.getIdServer(our_request.getHostName(), our_request.get_port())) + our_request.get_path_to_send();
	if (our_request.is_cgi_request() && !access(tmp.c_str(), F_OK | R_OK))
	{
		std::cout << BMAGENTA << "Dealing with CGI\n" << RESET;
		CGI	*my_cgi = new CGI(our_request, srv_events_list[i].data.fd);
		try
		{
			my_cgi->execute(epoll_fd);
			cgi_by_fd[my_cgi->get_pipe_out_fd()] = my_cgi;
			epoll_ctl(epoll_fd, EPOLL_CTL_DEL, srv_events_list[i].data.fd, NULL);
			pending_requests.erase(srv_events_list[i].data.fd);
			return;
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			delete my_cgi;
			our_request.set_response_code_message(502);
			reponse = our_request.create_response();
		}
	}
	else
	{
		std::cout << BMAGENTA<< "Dealing with standard request\n" << RESET;
		reponse = our_request.create_response();
	}
	// Générer la réponse (on devrait la stocker aussi dans un container)
	// std::string reponse = get_response("index.html");
	send(srv_events_list[i].data.fd, reponse.c_str(), reponse.size(), 0);
	// On a fini avec ce client
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, srv_events_list[i].data.fd, NULL);
	close(srv_events_list[i].data.fd);
	std::cout << BMAGENTA "Disconnected client: id[" << srv_events_list[i].data.fd << "]" << RESET << std::endl;
	pending_requests.erase(srv_events_list[i].data.fd);
}