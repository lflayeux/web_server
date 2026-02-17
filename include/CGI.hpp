#ifndef CGI_HPP
# define CGI_HPP

# include "../include/Request.hpp"
# include "../include/Response.hpp"

class Response;

class	CGI : public Response
{
	protected:
		char		**envp_;/* needed for excve() part*/
		Response	&our_response_;
		int			clientFd_;
		pid_t		pid_;
		int			pipeIn[2];
		int			pipeOut[2];
		bool		running;
		std::string	output_buffer_;
		std::map<std::string, std::string>	environnement_;
	public:
		CGI(Response &response, int client_fd) : envp_(NULL), our_response_(response), clientFd_(client_fd), pid_(-1), running(false), output_buffer_("")
		{
			if ((pipe(pipeIn) == -1) || (pipe(pipeOut) == -1))
				throw (std::runtime_error("Failed to create pipes"));
		};
		~CGI();
		void		build_environnement();
		void		convert_map_to_envp();
		void		show_env() const;
		std::string	get_script_path() const;
		std::string	execute(int epoll_fd);
		bool		read_output(std::string &out_response);
		std::string	parse_cgi_output(const std::string &output);
		bool		is_running() const;
		int			get_pipe_out_fd() const { return pipeOut[0]; }
		int			get_client_fd() const { return clientFd_; }
};

#endif