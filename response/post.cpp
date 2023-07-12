#include "../webserv.hpp"
#include "../config.hpp"

int fill_uplaod_file(HttpResponse &response, std::string &upload_path, std::string &file_name, std::vector<char> &content)
{
	std::ofstream file(file_name);
	if (file)
	{
		std::string str(content.begin(), content.end());
		file << str;
		std::string destination = upload_path + file_name;
		if (std::rename(file_name.c_str(), destination.c_str()))
		{
			ft_send_error(500, response);
			return(0);
		}
	}
	file.close();
	return (1);
}

void	upload_exist(HttpResponse& response, std::string& upload_path)
{
	std::string type_rep;
	std::string response_buffer;
	std::string file_name;

	type_rep = type_repo(upload_path);
	if ((response.request.files.empty() && response.content.empty()) 
		|| type_rep == "is_file" || type_rep == "not found")
	{
		ft_send_error(500, response);
		return ;
	}
	else
	{
		for(std::vector<File>::iterator file_it = response.request.files.begin(); file_it != response.request.files.end(); file_it++)
		{
			file_name = file_it->name;
			if(!fill_uplaod_file(response, upload_path, file_name, file_it->content))
				return ;
		}
		response.path_file = "www/201.html";
		fill_response(201, response);
		response.content_error = read_File_error(response.path_file);
		response.headers["content-length"] = ft_tostring(response.content_error.length());
		response_buffer = generate_http_response(response);
		response_buffer += response.content_error;
		send(response.fd, response_buffer.c_str(), response_buffer.length(), 0);
	}
}

int	upload_not_exist(HttpResponse& response)
{
	std::string type_rep;

	if (get_path(response))
	{
		type_rep = type_repo(response.path_file);
		if (type_rep == "is_file")
		{
			if (response.location_it->cgi.empty())
			{
				ft_send_error(403, response);
				return(0);
			}
			else
			{
				check_extention(response);
				if (response.cgi_it == response.location_it->cgi.end())
				{
					ft_send_error(403, response);
					return(0);
				}
			}
			fill_response(200, response);
			if (!response.request.content.empty())
    		{
				std::ofstream content("cgi.txt");
				if (!content.is_open())
					ft_send_error(500, response);
				std::string str(response.request.content.begin(), response.request.content.end());
       			content << str;
				content.close();
    		}
			execute_cgi(response);
			return(0);
		}
		else if (type_rep == "is_directory")
		{
			if (response_Http_Request(301, response))
			{
				// std::cout << "\033[31m" << response.path_file << "\033[00m" << std::endl;
				if (response.location_it->cgi.empty())
				{
					// std::cout << "response file = " << response.path_file << std::endl;
					ft_send_error(403, response);
				}
			// else
			// {
			// 	// std::cout << "!!!!!!!!!!!!!!!!!!!\n";
			// 	fill_response(200, response);
			// 	execute_cgi(response);
			// }
			}
		}
		else
			ft_send_error(500, response);
	}
	return (0);
}

std::string	generate_filename(std::string &file, int *num)
{
	// std::string	file_name = "file";
	// static int num = 0;

	std::string num_to_str = ft_tostring((*num)++);
	file += "_" + num_to_str;
	return (file);
}

void	add_extention(std::string& filename, HttpResponse& response)
{
	std::map<std::string, std::string> extention;

	extention["text/html"] = ".html";
	extention["text/css"] = ".css";
	extention["text/javascript"] = ".js";
	extention["image/png"] = ".png";
	extention["application/json"] = ".json";
	extention["application/xml"] = ".xml";
	extention["application/pdf"] = ".pdf";
	extention["image/jpeg"] = ".jpeg";
	extention["image/jpeg"] = ".jpeg";
	extention["image/gif"] = ".gif";
	extention["text/plain"] = ".text";
	extention["video/mp4"] = ".mp4";

	if (extention.find(response.request.headers["Content-Type"]) != extention.end())
		filename += extention[response.request.headers["Content-Type"]];
}

int response_post(HttpResponse& response)
{
	std::string upload_path;

	if (response.location_it->upload)
	{
		if (!response.location_it->dir.empty())
		{
			if (*response.location_it->dir.rbegin() != '/')
				upload_path = response.location_it->dir + "/";
			else
				upload_path = response.location_it->dir;
		}
		else if(!response.server_it->root.empty())
		{
			if (*response.location_it->dir.rbegin() != '/')
				upload_path = response.server_it->root + "/";
			else
			upload_path = response.server_it->root;
		}
		else
		{
			ft_send_error(404, response);
			return (0);
		}
		upload_exist(response, upload_path);
	}
	else
	{
		if (!upload_not_exist(response))
			return (1);

	}
	return (0);
}


	// if ((response.request.files.empty() && !response.content.empty()))
	// {
	// 	std::cout << YELLOW << "-----------------------> "<< END << std::endl;
	// 	file_name = generate_filename();
	// 	add_extention(file_name, response);
	// 	if(!fill_uplaod_file(response, upload_path, file_name, response.request.content))
	// 		return ;
	// 	// std::ofstream file(file_name);
	// 	// 	if (file)
	// 	// 	{
	// 	// 		file << file_it->content.data();
	// 	// 		if (std::rename(file_name.c_str(), destination.c_str()))
	// 	// 		{
	// 	// 			ft_send_error(500, response);
	// 	// 			return ;
	// 	// 		}
	// 	// 	}
	// 	// 	file.close();
	// }