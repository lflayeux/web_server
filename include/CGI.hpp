#ifndef CGI_HPP
# define CGI_HPP

# include "../include/Request.hpp"

class	CGI : public Config
{
	protected:
		std::map<std::string, std::string>	environnement_;
		char			**envp_;/* needed for excve() part*/
		const Request	&request_;
		const Config	&config_;
	public:
		CGI(const Request &request, const Config &config) : envp_(NULL), request_(request), config_(config) {};
		~CGI();
		void		build_environnement();
		void		convert_map_to_envp();
		void		show_env() const;
		std::string	get_script_path() const;
		std::string	execute();
		std::string	parse_cgi_output(const std::string &output);
};

#endif