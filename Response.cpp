#include "webserv.hpp"
#include "config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <cstdio>


std::string		res_content(int status_code, Config& config, HttpResponse& response)
{
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
	if (response.request.url == response.old_url)
		fill_response(status_code, response);
	else
		fill_response(302, response);
	switch (status_code)
	{
		case 301:
			if (res_content_dir(status_code, config, response))
				return (1);
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
		return (response_redirect(response, config));
	if (get_path(config, response))
	{
		type_rep = type_repo(response.path_file);
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
			ft_send_error(404, config, response);
	}
	return (0);
}

int	response_redirect(HttpResponse& response, Config& config)
{
	std::string type_rep;
	std::string	response_buffer;

	response.path_file = response.location_it->creturn.to;
	type_rep = type_repo(response.path_file);
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
		send(response.fd, response_buffer.c_str(), response_buffer.size(), 0);
	};
	return (0);
}

int response_post(Config& config, HttpResponse& response)
{
	std::string upload_path = "uploads";
	std::string type_rep;
	std::string response_buffer;

	type_rep = type_repo(upload_path);
	if (type_rep == "is_file" || type_rep == "not found")
	{
		ft_send_error(404,config, response);
		return (0);
	}
	std::string file_name = generate_filename();
	std::ofstream file(file_name);
	if (file)
	{
		file << response.request.content;
		std::string destination = "uploads/" + file_name;
		if(!(std::rename(file_name.c_str(), destination.c_str())))
		{
			response.path_file = "www/201.html";
			fill_response(201, response); // add 201 at status code 
			response.content = read_File_error(response.path_file);
			response.headers["content-length"] = ft_tostring(response.content.length());
			response_buffer = generate_http_response(response);
			response_buffer += response.content;
			send(response.fd, response_buffer.c_str(), response_buffer.length(), 0);
		}
	}
	else
		ft_send_error(404,config, response);
	return (0);
}

std::string	generate_filename()
{
	std::string	file_name = "file";
	static int num = 0;
	std::string num_to_str = ft_tostring(num++);
	// num++;
	// std::cout << "num_to_str = "<<num_to_str << std::endl;

	file_name += "_" + num_to_str;
	return (file_name);
}