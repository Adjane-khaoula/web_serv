#include "webserv.hpp"
#include "config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>


std::string		res_content(int status_code, Config& config, HttpResponse& response)
{
	// std::vector<Server>::iterator server_it = server(config, request);
	
	for (std::vector<ErrorPage>::iterator it = response.server_it->error_pages.begin(); it != response.server_it->error_pages.end(); it++)
	{
		if (it->error_code == status_code)
			return(read_File_error(it->page));
	}
	for (std::vector<ErrorPage>::iterator it = config.default_error_pages.begin(); it != config.default_error_pages.end(); it++)
	{		
		if (it->error_code == status_code)
			return (read_File_error(it->page));
	}
	return ("not found");
}

int response_Http_Request(int status_code , Config& config, HttpResponse& response)
{
	// std::cout << "response.request.url = " << response.request.url << std::endl;
	// std::cout << "response.old_url = " << response.old_url << std::endl;
	if (response.request.url == response.old_url)
		fill_response(status_code, response);
	else
		fill_response(302, response);
	switch (status_code)
	{
		case 301:
			if (res_content_dir(status_code, config, response))
				return (1);
			// response.head    ers["location"] = "https://profile.intra.42.fr/";
			break;
		case 200:
			return (1);
	}
	return (0);
}

int	response_get(Config& config, HttpResponse& response)
{
	std::string type_rep;

	if (!response.location_it->creturn.to.empty())
	{
		// std::cout << "@@@@@@@@@@@@@@@@@@" << std::endl;
		return (response_redirect(response, config));
	}
	// {
		// if (response.creturn.code)
		// 	fill_response(response.creturn.code, response);
		// else
		// 	fill_response(302, response);
		// response.headers["location"] = response.creturn.to;
		// response_buffer = generate_http_response(response);
		// send(response.fd, response_buffer.c_str(), response_buffer.length(), 0);
		// return (0);
	// }
	if (get_path(config, response))
	{
		type_rep = type_repo(response.path_file);
		std::cout << "type == " << type_rep << std::endl;
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
			// std::cout << "++++++++++++>" << response.path_file << std::endl;
			if (response_Http_Request(301,config, response))
				return (1);
		}
		else
			ft_send_error(404, config, response);
	}
	// ft_send_error(404, config, response);
	return (0);
}

int	res_content_dir(int status_code, Config& config, HttpResponse& response)
{
	std::vector<std::string>			content;
	std::vector<std::string>::iterator	content_it;
	std::string							response_buffer;
	// std::string::iterator				url_it = response.request.url.end();
	
	(void) status_code;
	if (content_dir(response.path_file, response) == "found")
	{
		if (!response.location_it->index.empty())
		{
			content_it = std::find(content.begin(), content.end(), response.location_it->index);
			if (content_it != content.end())
			{
				if (*response.path_file.rbegin() != '/')
					response.path_file += "/" + response.location_it->index;
				else
				{
					response.code = 200;
					response.reason_phrase = "ok";
					response.path_file += response.location_it->index;
				}
				// get_path(config, response);
				response.headers["Content-Type"] = get_content_type(response.path_file);
				return(1) ;
			}
		}
		content_it = std::find(content.begin(), content.end(), "index.html");
		if (content_it != content.end())
		{
			// std::cout << "@@@@@@@@@@@@@@@@@@" << std::endl;
			// std::cout << "*********> {"<< response.location_it->target << "}" << std::endl;
			if (*response.path_file.rbegin() != '/')
				response.path_file += "/index.html";
			else
			{
				response.code = 200;
				response.reason_phrase = "ok";
				response.path_file += "index.html";
			}
			// std::cout << "response.path_file = " << response.path_file << std::endl;
			response.headers["Content-Type"] = get_content_type(response.path_file);
			return(1) ;
		}
		if (response.location_it->autoindex)
		{
			// for (std::vector<std::string>::iterator it = content.begin(); it != content.end(); it++)
			// 	response.content += *it + "\n";
			// response.headers["Content-Length"] = ft_tostring(response.content.length());
			// response_buffer = generate_http_response(response);
			// response_buffer += response.content;
			// send(response.fd, response_buffer.c_str(), response_buffer.length(), 0) ;
			// if (*response.path_file.rbegin() != '/')
			std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
			response.path_file = "content_dir.html";
			fill_response(200, response);
			// else
				// response.path_file += "content_dir.html";
			return (1);
		}
		else
		{
			ft_send_error(403, config, response);
			return(0);
		}
	}
	ft_send_error(404, config, response);
	return(0);
}
