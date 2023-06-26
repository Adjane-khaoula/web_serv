 #include "../webserv.hpp"
#include "../config.hpp"

int	send_response(int fd, HttpRequest& request, HttpResponse& response, int status_code, bool *close_connexion)
{
	if (!request.method.empty())
	{
		response.old_url = request.url;
		// std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
		init_response(config, response, request, fd);
		if (new_request(request, response, status_code))
			return (1);
	}
	else
	{
		continue_previous_response(response);
		// std::cout << "*********************> " << response.finish_reading << std::endl;
		// response.finish_reading = 1;
		if(response.finish_reading)
		{
			if (request.headers["connection"] == "keep-alive")
				*close_connexion = false;
			return (1);
		}
	}
	return (0);
}

int new_request(HttpRequest &request, HttpResponse &response, int status_code) {

	std::string response_buffer;
	std::string content_length;

	if (!status_code)
	{
		status_code = check_req_line_headers(request);
		if (status_code == 1)
		{
			if (response_get(config, response))
			{
				read_File(response);
				content_length = ft_tostring(response.size_file);
				if (response.size_file == 0)
				{
					ft_send_error(404, config, response);
					return (1);
				} 
				else
				{
					response.headers["content-length"] = content_length;
					response_buffer = generate_http_response(response);
					std::cout << "\033[33m" << "{" << response_buffer << "}" << "\033[0m"  << std::endl;
					send(response.fd, response_buffer.c_str(), response_buffer.length(), 0);
				}
			}
			else
				return (1);
		}
		// else if (status_code == 2)3
		// {
		// 	if(!response_post(config, response))
		// 		return (1);
		// }
		// else if (status_code == 3)
		// {
		// 	if(!response_delete(config, response))
		// 		return (1);
		// }
	}
	else
	{
		ft_send_error(status_code, config, response);
			return (1);
	}
	return (0);
}

int continue_previous_response(HttpResponse &response) {

	// std::vctor
	// response.content.insert(response.content.end(), ) read_File(response);
	// response.content = "123456789";
	read_File(response);
	if (!response.finish_reading)
	{
			// std::cout << "\033[33m" << "{" << response.content.data() << "}" << "\033[0m"  << std::endl;
		// std::cout << "\033[33m" << "{" << response.content<< "}" << "\033[0m"  << std::endl;
			// std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
			send(response.fd, response.content.data(), response.content.size(), 0);
			// int ret = 
			// if (ret == -EAGAIN)
				return 0;
			// std::cout << "------------------SEND FAILED---------------------------------" << std::endl;
		// if (!send(response.fd, response.content.c_str(), response.content.length(), 0))
	}
	return 1;
}