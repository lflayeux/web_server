#include "../include/Request.hpp"
#include "../include/CGI.hpp"
#include "../include/Response.hpp"
#include "../include/Config.hpp"

// void handle_content_type(const std::string &request, Request &our_request)
// {

// }

void handle_multipart(const std::string &request, Request &our_request)
{
	(void)our_request;
	std::string 		line;
	std::string			boundary;
	std::string			body;

	std::string content_line = "Content-Type: multipart/form-data; boundary=";
	size_t start_boundaries = request.find(content_line);
	if (start_boundaries != std::string::npos)
	{
		start_boundaries += content_line.length();
		size_t end_boundaries = request.find("\r\n", start_boundaries);
		boundary = request.substr(start_boundaries, end_boundaries - start_boundaries);
	}
	std::cerr << BRED << boundary << RESET << std::endl;
	boundary = "--" + boundary;
	std::cerr << BRED << boundary << RESET << std::endl;
	size_t start_body = request.find(boundary);
	start_body = request.find("\r\n\r\n", start_body);
	if (start_body != std::string::npos)
	{
		boundary = boundary + "--";
		start_body += 4;
		size_t end_body = request.find(boundary, start_body);
		body = request.substr(start_body, end_body - start_body);

	}

	our_request.add_body(body);
	std::cout << BBLUE << "Body content: [" << body << "]\n" << RESET;
	
	std::string filename_value;
	size_t start_filename = request.find("filename=\"");
	if (start_filename != std::string::npos)
	{
		start_filename += 10;
		size_t end_filename = request.find("\"\r\n", start_filename);
		filename_value = request.substr(start_filename, end_filename - start_filename);
		std::cout << BRED << filename_value << RESET << std::endl;
	}
	our_request.add_header("filename", filename_value);
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
                    std::string port_str = header_value.substr(port_pos + 1);
					our_request.set_port(static_cast<int>(std::strtol(port_str.c_str(), NULL, 10)));
                }
            }
            if (header_name == "Content-Length")
				our_request.set_content_length(header_value);
			if (header_name == "Content-Type")
			{
				if (header_value.find("multipart") != std::string::npos)
				{
					handle_multipart(request, our_request);
					return (0);
				}
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
	return 0;
}