#include "webserv.hpp"
#include "config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>

std::string generate_http_response(HttpResponse &res)
{
	std::stringstream res_str;
	res_str << res.version << " " << res.code<< " " << res.reason_phrase << HTTP_DEL;
	for (std::map<std::string, std::string>::iterator it = res.headers.begin(); it != res.headers.end(); it++)
		res_str << it->first << ": " << it->second << HTTP_DEL;
	res_str << HTTP_DEL;
	// std::cout << "*********** {" << res_str.str() << "}" << std::endl; 
	// res_str << res.content;
	return res_str.str();
}

// std::string res_content_dir(int status_code, HttpRequest& request, Config& config, HttpResponse& response, std::string path)
// {
// 	std::vector<std::string> content;
// 	std::vector<std::string>::iterator it;
	
// 	if (content_dir(path, content) == "found")
// 	{
// 		if (!response.it2->index.empty())
// 		{
// 			it = std::find(content.begin(), content.end(), response.it2->index);
// 			if (it != content.end())
// 			{
// 				request.url += "/" + response.it2->index;
// 				response_get(request, config, response);
// 				return("") ;
// 			}
// 		}
// 		it = std::find(content.begin(), content.end(), "index.html");
// 		if (it != content.end())
// 		{
// 			request.url += "/index.html";
// 			response_get(request, config, response);
// 			return("") ;
// 		}
// 		if (response.it2->list_dir_content)
// 		{
// 			for (std::vector<std::string>::iterator it = content.begin(); it != content.end(); it++)
// 				response.content += *it + "\n";
// 			return (response.content);
// 		}
// 		else
// 		{
// 			response_Http_Request_error(403,request, config, response);
// 			return("");
// 		}
// 	}
// 	response_Http_Request_error(400, request, config, response);
// 	return("");

// }

std::string		res_content(int status_code, HttpRequest& request, Config& config, HttpResponse& response)
{
	std::vector<Server>::iterator server_it = server(config, request);
	std::vector<Location>::iterator location_it = location(config, request, server_it);
	
	for (std::vector<ErrorPage>::iterator location_it = response.it->error_pages.begin(); location_it != response.it->error_pages.end(); location_it++)
	{
		std::cout <<"*****************>"<< status_code << std::endl;
		if (location_it->error_code == status_code)
			return(read_File_error(location_it->page));
	}
	for (std::vector<ErrorPage>::iterator server_it = config.default_error_pages.begin(); server_it != config.default_error_pages.end(); server_it++)
	{		
		if (server_it->error_code == status_code)
			return (read_File_error(server_it->page));
	}
	return ("not found");
}

// std::string	res_content_file(int status_code,int fd, Config& config, HttpResponse& response)
// {
// 	if (status_code == 200)
// 	{
// 		std::string content = read_File(path);
// 		if (content == "not found")
// 		{
// 			response_Http_Request_error(404, request, config, response);
// 			return ("");
// 		}
// 		else
// 			return (content);
// 	}
// }

void response_Http_Request(int status_code, int fd, Config& config, std::map<int,HttpResponse>& responses)
{
	responses[fd].version = responses[fd].request.version;
	responses[fd].code = status_code;
	switch (status_code)
	{
		// case 301:
		// 	response.reason_phrase = "Moved Permanently";
		// 	if (res_content_dir(status_code,fd , config, response).empty())
		// 		return ;
			// response.headers["Content-Type"] = "text/plain";
			// response.headers["location"] = "https://profile.intra.42.fr/";
			break;
		case 200:
			responses[fd].reason_phrase = "ok";
			responses[fd].headers["Content-Type"] = get_content_type(responses[fd].request);
			responses[fd].headers["Connection"] = "keep-alive";
			break;
	}
}


void	response_get(int fd, Config& config, std::map<int,HttpResponse>& responses)
{

		// std::string path;
		std::string type_rep;

		if (!responses[fd].it2->dir.empty())
			responses[fd].path_file = responses[fd].it2->dir + responses[fd].request.url.substr(responses[fd].it2->target.length(), responses[fd].request.url.length());
			// path = response.it2->dir + req.url.substr(response.it2->target.length(), req.url.length());
		else if (!responses[fd].it->root.empty())
			responses[fd].path_file= responses[fd].it->root + responses[fd].request.url.substr(responses[fd].it2->target.length(), responses[fd].request.url.length());
		type_rep = type_repo(responses[fd].path_file);
		// std::cout << "path == " << path << std::endl;
		std::cout << type_rep << std::endl;
		if (type_rep == "is_file")
		{
			if (responses[fd].it2->cgi.empty())
			{
				response_Http_Request(200, fd, config, responses);
				return ;
			}
		}
		// else if (type_rep == "is_directory")
		// {
		// 	// std::cout << "/////////////////////////////" << std::endl;
		// 	response_Http_Request(301, req, config, response, path);
		// }
		// 	// else if (type_repo(path) == "is_directory with /")
			// {
			// 	path = path.substr(0, path.length() - 1);
			// 	response_Http_Request()
			// }
			// }
}


// default url is "/" sot we shod met location "/"
// add return redirection
// add client_max_body_size 10M
// autoindex = on Turn on or off directory listing.
//and Set a default file to answer if the request is a directory.
