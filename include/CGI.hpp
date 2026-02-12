#ifndef CGI_HPP
# define CGI_HPP

# include "../include/Request.hpp"
# include "../include/Response.hpp"

class	CGI : public Config
{
	protected:
		std::map<std::string, std::string>	environnement_;
		char			**envp_;/* needed for excve() part*/
		const Response	&our_response_;
	public:
		CGI(const Response &response) : envp_(NULL), our_response_(response) {};
		~CGI();
		void		build_environnement();
		void		convert_map_to_envp();
		void		show_env() const;
		std::string	get_script_path() const;
		std::string	execute();
		std::string	parse_cgi_output(const std::string &output);
};

#endif