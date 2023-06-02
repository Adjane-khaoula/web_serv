#include "webserv.hpp"
#include "config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

int ft_atof(std::string s) {
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

std::vector<Server>::iterator server(Config& config, HttpRequest& request)
{
	int port = ft_atof(request.headers["Host"].substr(request.headers["Host"].find(":") + 1, request.headers["Host"].length()));
	for (std::vector<Server>::iterator it = config.servers.begin(); it != config.servers.end(); it++)
		if (it->port == port)
			return (it);
	return (config.servers.end());
}

std::string read_File(std::string Path)
{
	std::ifstream file(Path);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}
// if => no location match the request uri => 404
int	check_req_well_formed(HttpRequest &req,Config& config)
{
	std::map<std::string, std::string>::iterator it;
	std::vector<Server>::iterator it2 = server(config, req);
	std::vector<Location>::iterator it3;
	std::string client_max_body_size = "10M";//the maximum size limit in megabytes (MB) for the request body.
	std::string client_max_body = client_max_body_size.substr(0, client_max_body_size.find("M"));//10

	it = req.headers.find("Transfer-Encoding");
	if (it != req.headers.end() && it->second.erase(0, 1) != "chunked")
		return (501);//not implemented
	if (it == req.headers.end()
		&& req.headers.find("Content-Length") == req.headers.end() && req.method == "POST")
		return(400);//Bad Request
	if (req.url.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%")
			!= std::string::npos)
		return(400);//Bad Request
	if (req.url.length() > 2048)
		return (414);//Request-URI Too Long
	if (ft_atof(req.headers["Content-Length"]) > ft_atof(client_max_body) * 1024 * 1024)
		return (413); //413 Request Entity Too Large
	for(it3 = it2->routes.begin(); it3 != it2->routes.end(); it3++)
	{
		if (it3->root == req.url)
			break;
	}
	if (it3 == it2->routes.end())
	{
		std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++returned 200 :)" << std::endl;
		return (404);
	}
	return(200);
}

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


std::string	res_content(int status_code, HttpRequest& request, Config& config)
{
	std::vector<Server>::iterator it = server(config, request);
	if (it != config.servers.end())
	{
		for (std::vector<ErrorPage>::iterator it2 = it->error_pages.begin();it2 != it->error_pages.end(); it2++)
			if (it2->error_code == status_code)
			{
				std::cout << "hna" << std::endl;
				return(read_File(it2->page));
			}
		for (std::vector<ErrorPage>::iterator it = config.default_error_pages.begin(); it != config.default_error_pages.end(); it++)
			if (it->error_code == status_code)
			{
				std::cout << "la hna" << std::endl;
				return (read_File(it->page));
			}
	}
	return ("");
}

HttpResponse response_Http_Request(int status_code, HttpRequest& request, Config& config)
{
	HttpResponse response;
	response.version = request.version;
	response.code = status_code;
	// std::cout << "/////////////////////////" << status_code << std::endl;
	switch(status_code) {
		case 501 :
			response.reason_phrase = "not implemented";
			response.content = res_content(status_code, request, config);
			if (response.content.empty())
				response.content = read_File("www/501.html");
			response.headers["Content-Type"] = "text/html";
			break ;
		case 400:
			response.reason_phrase = "Bad Request";
			response.content = res_content(status_code, request, config);
			if (response.content.empty())
				response.content = read_File("www/400.html");
			response.headers["Content-Type"] = "text/html";
			break ;
		case 414:
			response.reason_phrase = "Request-URI Too Long";
			response.content = res_content(status_code, request, config);
			if (response.content.empty())
				response.content = read_File("www/414.html");
			response.headers["Content-Type"] = "text/html";
			break ;
		case 413:
			response.reason_phrase = "Request Entity Too Large";
			response.content = res_content(status_code, request, config);
			if (response.content.empty())
				response.content = read_File("www/413.html");
			response.headers["Content-Type"] = "text/html";
			break ;
		case 404:
			response.reason_phrase = "Not Found";
			response.content = res_content(status_code, request, config);
			if (response.content.empty())
				response.content = read_File("www/404.html");
			response.headers["Content-Type"] = "text/html";
		case 200:
			response.reason_phrase = "ok";
			response.content = res_content(status_code, request, config);
			if (response.content.empty())
				response.content = "ara chi boysa";
			response.headers["Content-Type"] = "text/html";
			break ;
	}
	response.headers["Content-Length"] = std::to_string(response.content.length());
	return response;
}

// default url is "/" sot we shod met location "/"