#include "webserv.hpp"
#include "config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

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

std::string get_content_type(HttpRequest& req)
{
	if (req.url.substr(req.url.find(".") + 1,req.url.length()) == "html"
		|| req.url.substr(req.url.find(".") + 1,req.url.length()) == "htm"
		|| req.url.substr(req.url.find(".") + 1,req.url.length()) == "shtml")
		return("text/html");
	if (req.url.substr(req.url.find(".") + 1,req.url.length()) == "css")
		return("text/css");
	if (req.url.substr(req.url.find(".") + 1,req.url.length()) == "js")
		return("text/javascript");
	if (req.url.substr(req.url.find(".") + 1,req.url.length()) == "png")
		return("image/png");
	if (req.url.substr(req.url.find(".") + 1,req.url.length()) == "json")
		return("application/json");
	if (req.url.substr(req.url.find(".") + 1,req.url.length()) == "xml")
		return("application/xml");
	if (req.url.substr(req.url.find(".") + 1,req.url.length()) == "pdf")
		return("application/pdf");
	if (req.url.substr(req.url.find(".") + 1,req.url.length()) == "jpeg"
		|| req.url.substr(req.url.find(".") + 1,req.url.length()) == "jpg")
		return ("image/jpeg");
	if (req.url.substr(req.url.find(".") + 1,req.url.length()) == "gif")
		return ("image/gif");
	if (req.url.substr(req.url.find(".") + 1,req.url.length()) == "txt")
		return ("text/plain");
	return("application/octet-stream");
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
	for (std::vector<Location>::iterator it2 = server->routes.begin(); it2 != server->routes.end();it2++)
		if (req.url.find(it2->target) != std::string::npos && req.url.find(it2->target) == 0)
			return (it2);
	return (server->routes.end());
}

std::string read_File(std::string Path)
{
	std::cout << "---------------->path =="<< Path<<std::endl;
	std::ifstream file(Path);//if_open
	std::stringstream buffer;

	if (!file)
		return ("not found");
	buffer << file.rdbuf();
	return buffer.str();
}


//srckjbfk in uri ^