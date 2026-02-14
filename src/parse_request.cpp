#include "../include/Request.hpp"
#include "../include/CGI.hpp"
#include "../include/Response.hpp"
#include "../include/Config.hpp"

void handle_file(const std::string &request, Request &our_request)
{
	(void) our_request;
	std::string line;
	std::istringstream iss(request);
	std::string			boundary;
	bool				flag = false;

	while (std::getline(iss, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.find("Content-Type: multipart") != std::string::npos && flag == false)
		{
			std::cerr << "file found\n" << std::endl;
			size_t boundary_pos = line.find("boundary=");
            if (boundary_pos != std::string::npos)
			{
				boundary = line.substr(boundary_pos + 9);
				std::cerr << "boundary: " << boundary << std::endl;
				std::getline(iss, line);
				flag = true;
			}
		}
		else if (line.find(boundary) != std::string::npos && flag == true)
		{
			std::cerr << BYELLOW << ">>>>>> " << line << " <<<<<<\n" << RESET;
			std::getline(iss, line);
			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.size() - 1);
			std::string value;
			value = line.substr(line.find("filename=") + 10);
			value.erase(value.size() - 1);
			our_request.add_header("filename", value);
			std::cerr << BYELLOW << ">>>>> " << value << " <<<<<\n" << RESET;
			break;
		}
	}
	// on cherche le body
	std::string body_str;
	while (line != "\r")
		(std::getline(iss, line));
	while ((std::getline(iss, line)))
	{
		if (line.find(boundary) != std::string::npos)
		{
			break;
		}
		else
		{
			if (line[line.size() - 1] == '\r')
				line.erase(line.size() - 1);
			body_str += line + '\n';	
		}
	}
	our_request.add_body(body_str);
	std::cout << BBLUE << "BODY STR: [" << body_str << "]\n" << RESET;
}

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
					our_request.setHostname(header_value.substr(0, port_pos));
					std::cout << UGREEN << our_request.getHostName();
                    std::string port_str = header_value.substr(port_pos + 1);
					our_request.set_port(static_cast<int>(std::strtol(port_str.c_str(), NULL, 10)));
                }
            }
            else if (header_name == "Content-Length")
				our_request.set_content_length(header_value);
			else if (header_name == "Content-Type")
			{
				std::cerr << BRED << "Content-Type recieved\n" << RESET;
				handle_file(request, our_request);
				std::cout << "Our request is : " << our_request << std::endl;
				return (0);
			}
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