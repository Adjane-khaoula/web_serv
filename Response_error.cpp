#include "webserv.hpp"
#include "config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

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
	if (ft_atoi(req.headers["Content-Length"]) > ft_atoi(client_max_body) * 1024 * 1024)
		return (413); //413 Request Entity Too Large
	for(it3 = it2->routes.begin(); it3 != it2->routes.end(); it3++)
	{
		std::cout << "************** " << it3->dir << std::endl;
		if (it3->dir == req.url)
			break;
	}
	if (it3 == it2->routes.end())
		return (404);
	// if (it2->)
	return(301);
}

HttpResponse response_Http_Request_error(int status_code, HttpRequest& request, Config& config)
{
	HttpResponse response;
	response.version = request.version;
	response.code = status_code;
	// std::vector<Server>::iterator it2 = server(config, request);
	// std::vector<Location>::iterator it3;

	// for(it3 = it2->routes.begin(); it3 != it2->routes.end(); it3++)
	// {
	// 	if (it3->root == request.url)
	// 	{
	// 		for(std::vector<Redirection>::iterator it2 = it3->redirections.begin(); it2 != it3->redirections.end(); it2++)
	// 			{
	// 				std::cout <<"***************"<< it2->from << std::endl;
	// 				std::cout <<"***************"<< it2->to << std::endl;
	// 			}
	// 		break;
	// 	}
	// }
	
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
			break ;
		// case 405:
		// 	response.reason_phrase = "Method Not Allowed";
		// 	response.content = res_content(status_code, request, config);
		// 	if (response.content.empty())
		// 		response.content = read_File("www/404.html");
		// 	response.headers["Content-Type"] = "text/html";
		case 200:
			response.reason_phrase = "ok";
			response.content = res_content(status_code, request, config);
			if (response.content.empty())
				response.content = "ara chi boysa";
			response.headers["Content-Type"] = "text/html";
			break ;
	}
	if (status_code == 301)
		response_Http_Request (status_code, request, config, response);
	response.headers["Content-Length"] = std::to_string(response.content.length());
	return response;
}