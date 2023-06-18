#include "webserv.hpp"
#include "config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <cstdio>
#include <cstdlib>


void	del_content_dir(Config config, HttpResponse& response, std::string& path_dir)
{
	DIR* directory = opendir(path_dir.c_str());

	std::string file_path;

	if (directory)
	{
		struct dirent* content_dir;

		while ((content_dir = readdir(directory)))
		{
			file_path = response.path_file + content_dir->d_name;
			type_rep = type_repo(file_path)
			if (type_rep == "is_file")
				unlink(file_path);
			else if (type_rep == "id_directory")
			{
				del_content_dir(file_path);
				rmdir(file_Path.c_str());
			}
			else
				ft_send_error(403, con fig, response);
		}
	}
	else
	{
		ft_send_error(403,config, response);
		return ;
	}
	///check if all deleted
	closedir(dir);
	ft_send_error(204, config, response);
	rmdir(file_dir.c_str())
	// response.path_file = "www/409.html";
	// response.content = read_File_error("www/409.html");
	// response.headers["Content-Length"] = ft_tostring(response.content.length());
	// fill_responser(409, response);
	// response_buffer = generate_http_response(response);
	// send(response.fd, response_buffer.c_str(), response_buffer.length(), 0);
}

int response_delete(Config& config, HttpResponse& response)
{
	std::string type_rep;
	std::string response_buffer;
	if (get_path(config, response))
	{
		type_rep = type_repo(response.path_file)
		if (type_rep == "is_file")
		{
			if (response.location_it->cgi.empty())
			{
				unlink(response.path_file.c_str());
				ft_send_error(204, config, response);
			}
		}
		if(type_rep == "is_directory")
		{
			if (*response.path_file.rbegin() != '/')
			{
				response.path_file = "www/409.html";
				response.content = read_File_error("www/409.html");
				response.headers["Content-Length"] = ft_tostring(response.content.length());
				fill_responser(409, response);
				response_buffer = generate_http_response(response);
				send(response.fd, response_buffer.c_str(), response_buffer.length(), 0);
			}
			else if (response.location_it->cgi.empty())
			{
				del_content_dir();
				ft_send_error(403, config, response);
			}
		}
		else
			ft_send_error(404, config, response);
	}
	else
		ft_send_error(404, config, response);
	return(0);
}
