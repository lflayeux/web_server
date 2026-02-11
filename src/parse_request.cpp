#include "../include/Request.hpp"
#include "../include/CGI.hpp"
#include "../include/Response.hpp"
#include "../include/Config.hpp"

int parse_request(const std::string &request, Request &our_request)
{
	std::string line;
	std::istringstream iss(request);

	/* 1. Parsing of the request line (METHOD) */
	if (std::getline(iss, line))
	{
		// we erase the char '\r' if it does exist in our first line
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		
		std::istringstream	line_stream(line);
		std::string			method, path, version;

		line_stream >> method >> path >> version;
		our_request.set_method(method);
		our_request.set_path(path);
		our_request.set_version(version);
	}

	while (std::getline(iss, line))
	{
		// we erase the char '\r' if it does exist in our first line
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.empty())
			break;
		std::cout << "<line> ["<< line << "]" << std::endl;

		// pour split "Header: Value"
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos)
        {
            std::string header_name = line.substr(0, colon_pos);
            std::string header_value = line.substr(colon_pos + 1);
            
            // Enlever les espaces au start de la value
            size_t start = header_value.find_first_not_of(" \t");
            if (start != std::string::npos)
                header_value = header_value.substr(start);
            
            // Stocker
            our_request.add_header(header_name, header_value);
            
            // Parser les headers importants
            if (header_name == "Host")
            {
                // "localhost:8080" -> extraire le port
                size_t port_pos = header_value.find(':');
                if (port_pos != std::string::npos)
                {
                    std::string port_str = header_value.substr(port_pos + 1);
					our_request.set_port(static_cast<int>(std::strtol(port_str.c_str(), NULL, 10)));
                }
            }
            else if (header_name == "Content-Length")
				our_request.set_content_length(header_value);
        }
	}
	// 3. PARSER LE BODY (tout ce qui reste)
	std::string body;
	std::string remaining;
	while (std::getline(iss, remaining))
	{
		body += remaining;
		if (iss.peek() != EOF)  // Pas la dernière ligne
			body += "\n";
	}
	our_request.add_body(body);
	std::cout << "Our request is : " << our_request << std::endl;
	return 0;
}