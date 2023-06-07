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
		else
		{
			response_Http_Request_error(403,request, config, response);
			return("");
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
	if (status_code == 200)
	{
		std::string content = read_File(path);
		if (content == "not found")
		{
			response_Http_Request_error(404, request, config, response);
			return ("");
		}
		else
			return (content);
	}
}

void response_Http_Request(int status_code, HttpRequest& request, Config& config, HttpResponse& response, std::string path)
{
	response.version = request.version;
	response.code = status_code;
	switch (status_code)
	{
		case 301:
			response.reason_phrase = "Moved Permanently";
			if (res_content_dir(status_code, request, config, response, path).empty())
				return ;
			// response.headers["Content-Type"] = "text/plain";
			// response.headers["location"] = "https://profile.intra.42.fr/";
			break;
		case 200:
			response.reason_phrase = "ok";
			if(res_content_file(status_code, request, config, response, path).empty())
				return ;
			else
			{
				response.content = res_content_file(status_code, request, config, response, path);
			}
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
		type_rep = type_repo(path);
		std::cout << type_rep << std::endl;
		if (type_rep == "is_file")
		{
			if (response.it2->cgi.empty())
			{
				response_Http_Request(200, req, config, response, path);
				return ;
			}
		}
		else if (type_rep == "is_directory")
				response_Http_Request(301, req, config, response, path);
		// else if (type_repo(path) == "is_directory with /")
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
