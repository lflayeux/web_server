# include "../include/CGI.hpp"

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
}

std::string	CGI::get_script_path() const
{
	return (our_response_.get_cgi_path(our_response_.get_path_to_send(), our_response_.getIdServer(our_response_.get_port())) + our_response_.get_script_name());
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

std::string	CGI::execute()
{
	build_environnement();
	convert_map_to_envp();

	std::cout << BGREEN << "Debug CGI, execute() starts\n" << RESET;
	/* On crée des pipes pour stdin et stdout */
	int	pipe_in[2];// pour envoyer le body
	int	pipe_out[2];// pour recevoir la réponse

	if (pipe(pipe_in) < 0 || pipe(pipe_out) < 0)
		throw std::runtime_error("function pipe() failed");
	
	/* On vient FORK()*/
	pid_t	pid = fork();

	if (pid < 0)
		throw std::runtime_error("function fork() failed");
	
	if (pid == 0)// on se situe dans le processus enfant
	{
		close(pipe_in[1]);
		dup2(pipe_in[0], STDIN_FILENO);
		close(pipe_in[0]);

		close(pipe_out[0]);
		dup2(pipe_out[1], STDOUT_FILENO);
		close(pipe_out[1]);

		char *av[2];
		std::string	script_path = get_script_path();
		// DEBUG
   		std::cerr << "DEBUG: Script path = [" << script_path << "]" << std::endl;
    	std::cerr << "DEBUG: File exists? " << (access(script_path.c_str(), F_OK) == 0 ? "YES" : "NO") << std::endl;
    	std::cerr << "DEBUG: File executable? " << (access(script_path.c_str(), X_OK) == 0 ? "YES" : "NO") << std::endl;
		
		
		av[0] = const_cast<char *>(script_path.c_str());
		av[1] = NULL;
		
		std::cerr << BGREEN << "Debug : just before EXECVE - script_path:" << script_path << RESET << std::endl;
		execve(av[0], av, envp_);

		std::cerr << "execve failed: " << strerror(errno) << std::endl;
		exit(1);
	}
	else// processus parent
	{
		close(pipe_in[0]);
		close(pipe_out[1]);

		if (our_response_.get_method() == "POST")// on veut envoyer le body dans stdin du CGI si jamais la method = POST
		{
			std::string body = our_response_.get_body();
			write(pipe_in[1], body.c_str(), body.size());
		}
		close(pipe_in[1]);

		std::string	output;
		char buffer[4096];
		ssize_t	bytes;

		while ((bytes = read(pipe_out[0], buffer, sizeof(buffer))) > 0)
			output.append(buffer, bytes);
		close(pipe_out[0]);

		int	status;
		waitpid(pid, &status, 0);

		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			throw std::runtime_error("CGI script has failed.");
		
		//DEBUG : Afficher l'output en brut
        std::cout << "=== CGI RAW OUTPUT ===" << std::endl;
        // std::cout << output << std::endl;
        std::cout << "======================" << std::endl;
        
		return (parse_cgi_output(output));
	}
	return ("");
}