#include "webserv.hpp"
#include "config.hpp"
#include <iostream>

int ft_atoi(std::string s) {
	int					sign = 1;
	int				  /*frac = 0.1,*/ nbr = 0;
	std::string::iterator it;

	for (it = s.begin(); *it == ' '; ++it)
		;
	if ((*it == '+' || *it == '-') && isdigit(*(it + 1)))
	{
		if (*it == '-')
			sign = -1;
		it++;
	}
	else if (*it == '+' && !isdigit(*(it + 1)))
		return (-1);
	while (it != s.end() && isdigit(*it))
		nbr = nbr * 10 + (*(it++) - '0');
	return (sign * nbr);
}

std::string	ft_tostring(int nbr)
{
	std::string	str;

	while (nbr > 10)
	{
		str.insert(0,1, static_cast<char>((nbr % 10) + '0'));
		nbr /= 10;
	}
	if (nbr > 0)
		str.insert(0,1, static_cast<char>(nbr+ '0'));
	return (str);
}

std::vector<Server>::iterator server(Config& config, HttpRequest& request)
{
	int	position = request.headers["Host"].find(":");
	std::string ip = request.headers["Host"].substr(0, position);
	int port = ft_atoi(request.headers["Host"].substr(position + 1, request.headers["Host"].length()));
	for (std::vector<Server>::iterator it = config.servers.begin(); it != config.servers.end(); it++)
		if (it->ip == ip && it->port == port)
			return (it);
	return (config.servers.begin());
}

std::vector<Location>::iterator location(Config& config, HttpRequest& req, std::vector<Server>::iterator server)
{
	unsigned long	length_location(0);
	std::vector<Location>::iterator location = server->routes.end();
	std::vector<Redirection>::iterator redirection_it;

	for (std::vector<Location>::iterator location_it = server->routes.begin(); location_it != server->routes.end();location_it++)
	{
		if (req.url.find(location_it->target) != std::string::npos)
		{
			if ((location_it->target.length()) > length_location)
			{
				length_location = location_it->target.length();
				location = location_it;
			}
		}
	}
	if (location != server->routes.end() && !location->redirections.empty())
	{
		for (redirection_it = location->redirections.begin(); redirection_it != location->redirections.end(); redirection_it++)
		{
			size_t find_from = req.url.find(redirection_it->from);
			size_t find_to = req.url.find(redirection_it->to);

			if (find_from != std::string::npos && find_to == std::string::npos)
			{
				req.url = req.url.substr(0,find_from) + redirection_it->to + req.url.substr((find_from + redirection_it->from.length()),req.url.length());
				break;
			}
		}
		// std::cout << "req befor rewrite = " << req.url << std::endl;
		// for (std::vector<Location>::iterator location_it = server->routes.begin(); location_it != server->routes.end();location_it++)
		// {
		// 	if (req.url.find(location_it->target) != std::string::npos)
		// 	{
		// 		if ((location_it->target.length()) > length_location)
		// 		{
		// 			length_location = location_it->target.length();
		// 			location = location_it;
		// 		}
		// 	}
		// }
	}
	// if (location != server->routes.begin())
	return (location);
	// return (server->routes.end());
}


std::string content_dir(std::string dir,HttpResponse& response)
{
	DIR* directory = opendir(dir.c_str());

	if (directory)
	{
		struct dirent* content_dir;
		std::ofstream file("content_dir.html");
		if (file)
		{
			file << "<!DOCTYPE html>\n" << "<html>\n" << "<head>\n" << "<title>autoindex</title>\n";
			file << "</head>\n" << "<body>\n";
			// file << "<head>\n";
			// file << "<title>autoindex</title>\n";
			// file << "</head>\n";
			// file << "<body>\n";
			file << "<h1>" << "Index of " << response.request.url <<"</h1>\n" << "<ul>\n";
			// file << "<ul>\n";
			while ((content_dir = readdir(directory)))
			{
				if (strcmp(content_dir->d_name, ".") && strcmp(content_dir->d_name , "..")
						&& strcmp(content_dir->d_name, ".DS_Store"))
				{
					if (*response.request.url.rbegin() != '/')
						file << "<li><a href=\"" << "http://"<< response.server_it->ip << ":" << response.server_it->port << response.request.url << '/' << content_dir->d_name << "\">" << content_dir->d_name << "</a></li>\n";
					file << "<li><a href=\"" << "http://"<< response.server_it->ip << ":" << response.server_it->port << response.request.url << content_dir->d_name << "\">" << content_dir->d_name << "</a></li>\n";
				}
			}
			file << "</ul>\n" << "</body>\n" << "</html>\n";
			// file << "</body>\n";
			// file << "</html>\n";
		}
		file.close();
		closedir(directory);
		return("found");
	}
	return ("not found");
}

void	ft_send_error(int status_code, Config config, HttpResponse& response)
{
	std::string		response_buffer;

	response_Http_Request_error(status_code, config, response);
	response_buffer = generate_http_response(response);
	response_buffer += response.content;
	send(response.fd, response_buffer.c_str(), response_buffer.length(), 0);
}



int	response_redirect(HttpResponse& response, Config& config)
{
	std::string type_rep;
	std::string	response_buffer;
	// std::cout <<"response.location_it->creturn.code = " << response.location_it->creturn.code << std::endl;
	// std::cout <<"response.location_it->creturn.to = " << response.location_it->creturn.to << std::endl;
	std::cout << "!!!!!!!!!!!!!!!!!!!!!" << std::endl;
	response.path_file = response.location_it->creturn.to;
	type_rep = type_repo(response.path_file);
	std::cout << "response.path_file = "<< response.path_file << std::endl;
	std::cout << "type_repo = "<< type_rep << std::endl;
	if (type_rep == "is_file")
	{
		if (response.location_it->cgi.empty())
		{
			response_Http_Request(200, config, response);
			return (1);
		}
	}
	else if (type_rep == "is_directory")
	{
		if (response_Http_Request(301,config, response))
			return (1);
	}
	else
	{
		if (response.location_it->creturn.code)
			fill_response(response.location_it->creturn.code, response);
		else
			fill_response(302, response);
		response.headers["location"] = response.location_it->creturn.to;
		response_buffer = generate_http_response(response);
		std::cout << "******> {" << response_buffer << "}"<< std::endl;
		send(response.fd, response_buffer.c_str(), response_buffer.size(), 0);
	};
	return (0);
}
