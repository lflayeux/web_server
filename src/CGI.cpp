# include "../include/CGI.hpp"


bool	CGI::is_running() const
{
	return (this->running);
}

static std::string	get_a_string(int value)
{
    std::ostringstream	oss;

    oss << value;
    return (oss.str());
}

void	CGI::build_environnement()
{
// 1. Variables standards
    environnement_["REQUEST_METHOD"] = our_response_.get_method();
    environnement_["CONTENT_LENGTH"] = get_a_string(our_response_.get_content_length());
    environnement_["PATH_INFO"] = our_response_.get_path_to_send();
    environnement_["SERVER_PROTOCOL"] = "HTTP/1.1";
    environnement_["GATEWAY_INTERFACE"] = "CGI/1.1";
    environnement_["SERVER_PORT"] = get_a_string(our_response_.get_port());

    // 2. Variables specifiques aux headers
    std::map<std::string, std::string> headers = our_response_.get_headers(); 
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
	{
        std::string key = "HTTP_" + it->first;
        for (size_t i = 0; i < key.length(); ++i)
		{
            key[i] = std::toupper(key[i]);
            if (key[i] == '-') key[i] = '_';
        }
        environnement_[key] = it->second;
    }
}

void CGI::convert_map_to_envp()
{
    // 1. Allouer le tableau de pointeurs
    //    +1 pour le NULL terminal (requis par execve)
    envp_ = new char*[environnement_.size() + 1];
    
    size_t i = 0;
    for (std::map<std::string, std::string>::const_iterator it = environnement_.begin();
         it != environnement_.end(); 
         ++it)
    {
        // 2. Construire la string "KEY=VALUE"
        std::string env_line = it->first + "=" + it->second;
        
        // 3. Allouer et copier la string
        envp_[i] = new char[env_line.length() + 1];
        std::strcpy(envp_[i], env_line.c_str());
        
        ++i;
    }
    
    // 4. Terminer avec NULL (obligatoire pour execve)
    envp_[i] = NULL;
}

void	CGI::show_env() const
{
	for (std::map<std::string, std::string>::const_iterator it = environnement_.begin(); it != environnement_.end(); it++)
	{
		std::cout << it->first << "=" << it->second << std::endl;
	}
}

CGI::~CGI()
{
    if (envp_)
    {
        for (size_t i = 0; envp_[i] != NULL; ++i)
            delete[] envp_[i];
        delete[] envp_;
    }
	if (running)
	{
		kill(pid_, SIGKILL);
		waitpid(pid_, NULL, 0);
	}
}

std::string	CGI::get_script_path() const
{
	return (our_response_.get_cgi_path(our_response_.get_path_to_send(), our_response_.getIdServer(our_response_.getHostName(), our_response_.get_port())) + our_response_.get_script_name());
}

std::string	CGI::parse_cgi_output(const std::string &output)
{

	// Le CGI peut retourner :
	// 1. Des headers + body (mode "parsed header")
	// 2. Juste un body (mode "non-parsed header", NPH)
	std::string line;
	std::string body;
	std::stringstream ss(output);
	while(std::getline(ss, line))
		body += line + "\r\n";
		
	size_t header_end = body.find("\r\n\r\n");
	if (header_end == std::string::npos)
	{
		return (body);  // Pas de headers, retour direct		
	}

	// Extraire headers et body
	std::string headers = body.substr(0, header_end);
	body = body.substr(header_end + 4);

	// Construire la réponse HTTP complète
	std::string response = "HTTP/1.1 200 OK\r\n";
	response += headers;
	response += "\r\n\r\n";
	response += body;

	return (response);

}

bool	CGI::check_script_path() const
{
	std::string	path = our_response_.get_path_to_send();
	size_t	dot_pos = path.find_last_of('.');
	if (dot_pos == std::string::npos)
		return (false);
	std::string	extension = path.substr(dot_pos);// exemple : .php ou .py
		
	std::vector<std::string> cgi_extensions = our_response_.get_cgi_extensions();
	std::string	script_path = get_script_path();
	for (size_t i = 0; i < cgi_extensions.size(); ++i)
	{
		if (extension == cgi_extensions[i])
		{
			if(cgi_extensions[i] == ".py")
			{
				if(script_path == "/usr/bin/python3")
					return true;
				else
					return false;
			}
			else
			{
				if(script_path == "/usr/bin/php")
					return true;
				else
					return false;
			}
		}
	}
	return (false);
}
 

std::string	CGI::execute(int epoll_fd)
{
	if(!check_script_path())
	{
		close(pipeIn[0]);
		close(pipeIn[1]);
		close(pipeOut[0]);
		close(pipeOut[1]);		
		throw std::runtime_error("Can't execute this file with this script file");
	}
		
	build_environnement();
	convert_map_to_envp();

	std::cout << UGREEN << "CGI -> execute() starts\n" << RESET;	

	/* On vient FORK()*/
	pid_ = fork();

	if (pid_ < 0)
		throw std::runtime_error("function fork() failed");
	
	if (pid_ == 0)// on se situe dans le processus enfant
	{
		// Les FD avec CLOEXEC (epoll, sockets, autres pipes) seront fermés automatiquement par execve
		dup2(pipeIn[0], STDIN_FILENO);
		dup2(pipeOut[1], STDOUT_FILENO);
		close(pipeIn[0]);
		close(pipeOut[1]);

		char *av[3];
		std::string	script_path = get_script_path();
		

		std::string pathToFind = our_response_.get_path_to_send();
		std::string tmp = our_response_.getRoot(pathToFind, our_response_.getIdServer(our_response_.getHostName(), our_response_.get_port())) + our_response_.get_path_to_send();
		av[0] = const_cast<char *>(script_path.c_str());
		av[1] = const_cast<char *>(tmp.c_str());
		av[2] = NULL;

		execve(av[0], av, envp_);

		std::cerr << BRED << "execve failed: " << strerror(errno) << RESET << std::endl;
		exit(1);
	}
	else if (pid_ > 0)// processus parent
	{
		close(pipeIn[0]);
		close(pipeOut[1]);
		running = true;

		int flags = fcntl(pipeOut[0], F_GETFL);
		fcntl(pipeOut[0], F_SETFL, flags | O_NONBLOCK);

		if (our_response_.get_method() == "POST")// on veut envoyer le body dans stdin du CGI si jamais la method = POST
		{
			std::string body = our_response_.get_body();
			write(pipeIn[1], body.c_str(), body.size());
		}
		close(pipeIn[1]);

		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = pipeOut[0];
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipeOut[0], &ev) < 0)
			throw std::runtime_error("Failed to add CGI pipe to epoll");
		return ("");
	}
	else
		throw (std::runtime_error("Fork failed"));
		
	return ("");
}



bool	CGI::read_output(std::string &out_response)
{
	out_response.clear();
	char buffer[4096];
	ssize_t bytes_read = 0;
	
	bytes_read = read(pipeOut[0], buffer, sizeof(buffer));
	if (bytes_read > 0)
	{
		output_buffer_.append(buffer, bytes_read);
		return (false);
	}
	else if (bytes_read == 0)
	{
		running = false;
		close(pipeOut[0]);
		waitpid(pid_, NULL, 0);
		if (output_buffer_.empty())
		{
			our_response_.set_response_code_message(502);
			out_response = our_response_.create_response();
		}
		else
			out_response = parse_cgi_output(output_buffer_);
		return (true);
	}
	else
	{
		return (false);
	}
}