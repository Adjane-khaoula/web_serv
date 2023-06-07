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
	res_str << res.content;
	return res_str.str();
}

std::string res_content_dir(int status_code, HttpRequest& request, Config& config, HttpResponse& response, std::string path)
{
	std::vector<std::string> content;
	std::vector<std::string>::iterator it;
	
	if (content_dir(path, content) == "found")
	{
		if (!response.it2->index.empty())
		{
			it = std::find(content.begin(), content.end(), response.it2->index);
			if (it != content.end())
			{
				request.url += "/" + response.it2->index;
				response_get(request, config, response);
				return("") ;
			}
		}
		it = std::find(content.begin(), content.end(), "index.html");
		if (it != content.end())
		{
			request.url += "/index.html";
			response_get(request, config, response);
			return("") ;
		}
		if (response.it2->list_dir_content)
		{
			for (std::vector<std::string>::iterator it = content.begin(); it != content.end(); it++)
				response.content += *it + "\n";
			return (response.content);
		}
	}
	response_Http_Request_error(400, request, config, response);
	return("");

}
std::string		res_content(int status_code, HttpRequest& request, Config& config, HttpResponse& response)
{
	for (std::vector<ErrorPage>::iterator it2 = response.it->error_pages.begin();it2 != response.it->error_pages.end(); it2++)
		if (it2->error_code == status_code)
			return(read_File(it2->page));
	for (std::vector<ErrorPage>::iterator it = config.default_error_pages.begin(); it != config.default_error_pages.end(); it++)
		if (it->error_code == status_code)
			return (read_File(it->page));
	return ("not found");
}

std::string	res_content_file(int status_code, HttpRequest& request, Config& config, HttpResponse& response, std::string path)
{
	// std::vector<Server>::iterator it = server(config, request);
	// std::vector<Location>::iterator it2 = location(config, request);
	if (status_code == 200)
	{
		std::string content = read_File(path);
	// 	std::string path;
	// 	if (!response.it2->dir.empty())
	// 	{
	// 		// path = response.it2->dir + request.url.substr(response.it2->target.length(), request.url.length());
	// 		// if (type_repo(path) == "is_file")
	// 		// {
	// 			// std::cout << "*******> " << path << std::endl;
	// content = read_File(path);
		if (content == "not found")
		{
			response_Http_Request_error(404, request, config, response);
			return ("");
		}
		else
			return (content);
	}
			// else if (type_repo(path) == "is_directory")
			// {
			// 	response_get()
			// }
		// }
		// else if (!response.it->root.empty())
		// {
		// 	path = response.it->root + request.url.substr(response.it2->target.length(), request.url.length());
		// 	content = read_File(path);
		// 	if (content == "not found")
		// 	{
		// 		response_Http_Request_error(404, request, config, response);
		// 		return ("");
		// 	}
		// 	else
		// 		return (content);
		// }
		// else
		// 	status_code = 404;
	// std::cout << "///////////" << status_code << std::endl;
	// for (std::vector<ErrorPage>::iterator it2 = response.it->error_pages.begin();it2 != response.it->error_pages.end(); it2++)
	// 	if (it2->error_code == status_code)
	// 		return(read_File(it2->page));
	// for (std::vector<ErrorPage>::iterator it = config.default_error_pages.begin(); it != config.default_error_pages.end(); it++)
	// 	if (it->error_code == status_code)
	// 		return (read_File(it->page));
	// return ("not found");
}

void response_Http_Request(int status_code, HttpRequest& request, Config& config, HttpResponse& response, std::string path)
{
	response.version = request.version;
	response.code = status_code;
	switch (status_code)
	{
		case 301:
			response.reason_phrase = "Moved Permanently";
			response.content = res_content_dir(status_code, request, config, response, path);
			if (response.content.empty())
				return ;
			// response.headers["Content-Type"] = "text/plain";
			response.headers["location"] = "https://profile.intra.42.fr/";
			break;
		case 200:
			response.reason_phrase = "ok";
			if(res_content_file(status_code, request, config, response, path).empty())
				return ;
			else
				response.content = res_content_file(status_code, request, config, response, path);
			response.headers["Content-Type"] = "text/html";
			break;
	}
	response.headers["Content-Length"] = std::to_string(response.content.length());
}


void	response_get(HttpRequest& req, Config& config, HttpResponse& response)
{

		std::string path;
		std::string type_rep;

		if (!response.it2->dir.empty())
			path = response.it2->dir + req.url.substr(response.it2->target.length(), req.url.length());
		else if (!response.it->root.empty())
			path = response.it->root + req.url.substr(response.it2->target.length(), req.url.length());
		std::cout << "************> path {" << path << "}"<< std::endl;
		type_rep = type_repo(path);
		std::cout << type_rep << std::endl;
		if (type_rep == "is_file")
		{
			if (response.it2->cgi.empty())
				response_Http_Request(200, req, config, response, path);
		}
		else if (type_rep == "is_directory")
		{
			std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ " << std::endl;
				response_Http_Request(301, req, config, response, path);
		}
		// else if (type_repo(path) == "is_directory with /")
		// {
		// 	path = path.substr(0, path.length() - 1);
		// 	response_Http_Request()
		// }
		// }
	// DIR* directory = opendir(req.url.substr(1,req.url.length()).c_str());
	// if (directory)
	// {
	// 	struct dirent* content_dir;
    //     while ((content_dir = readdir(directory))) 
    //         std::cout << "Entry name: " << content_dir->d_name << std::endl;
    //     closedir(directory);
    // }
	// else
	// 	std::cout << "/////////////////////////// ma t7alch\n";
}


// default url is "/" sot we shod met location "/"
// add return redirection
// add client_max_body_size 10M
// autoindex = on Turn on or off directory listing.
//and Set a default file to answer if the request is a directory.
