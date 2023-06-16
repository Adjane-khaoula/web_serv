#include "webserv.hpp"
#include "config.hpp"
#include <iostream>

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

std::string get_content_type(std::string path)
{
	std::map<std::string, std::string> content_type;
	std::string type = path.substr(path.rfind(".") + 1,path.length());

	content_type["html"] = "text/html";
	content_type["htm"] = "text/html";
	content_type["shtml"] = "text/html";
	content_type["css"] = "text/css";
	content_type["js"] = "text/javascript";
	content_type["png"] = "image/png";
	content_type["json"] = "application/json";
	content_type["xml"] = "application/xml";
	content_type["pdf"] = "application/pdf";
	content_type["xml"] = "application/xml";
	content_type["jpeg"] = "image/jpeg";
	content_type["jpg"] = "image/jpeg";
	content_type["gif"] = "image/gif";
	content_type["txt"] = "text/plain";
	content_type["mp4"] = "video/mp4";

	if (content_type.find(type) != content_type.end())
		return (content_type[type]);
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
	unsigned long	length_location(0);
	std::vector<Location>::iterator location = server->routes.end();
	std::vector<Redirection>::iterator redirection_it;

	for (std::vector<Location>::iterator location_it = server->routes.begin(); location_it != server->routes.end();location_it++)
	{
		if (req.url.find(location_it->target) != std::string::npos)
		{
			if ((location_it->target.length()) > length_location)
			{
				length_location = location_it->target.length();
				location = location_it;
			}
		}
	}
	if (location != server->routes.end() && !location->redirections.empty())
	{
		for (redirection_it = location->redirections.begin(); redirection_it != location->redirections.end(); redirection_it++)
		{
			size_t find_from = req.url.find(redirection_it->from);
			size_t find_to = req.url.find(redirection_it->to);

			if (find_from != std::string::npos && find_to == std::string::npos)
			{
				req.url = req.url.substr(0,find_from) + redirection_it->to + req.url.substr((find_from + redirection_it->from.length()),req.url.length());
				break;
			}
		}
		// std::cout << "req befor rewrite = " << req.url << std::endl;
		// for (std::vector<Location>::iterator location_it = server->routes.begin(); location_it != server->routes.end();location_it++)
		// {
		// 	if (req.url.find(location_it->target) != std::string::npos)
		// 	{
		// 		if ((location_it->target.length()) > length_location)
		// 		{
		// 			length_location = location_it->target.length();
		// 			location = location_it;
		// 		}
		// 	}
		// }
	}
	// if (location != server->routes.begin())
	return (location);
	// return (server->routes.end());
}

std::string read_File(HttpResponse& response)
{

	std::ifstream file;
	std::string res = "";
	std::stringstream hex;
	file.open(response.path_file, std::ifstream::binary);

	if (file.is_open())
	{
		file.seekg (0, file.end);
		int length = file.tellg();
		file.seekg (0, file.beg);
		std::vector<char> buffer(length);

		if (response.get_length == false)
		{
			response.get_length = true;
			return (ft_tostring(length));
		}
		if (response.byte_reading < length)
		{
			// int chunkSize = std::min(BUFF_SIZE, length - byte_reading);
			int chunkSize = std::min(length, length - response.byte_reading);
			hex << std::hex << chunkSize;
			buffer.resize(chunkSize);
			res += hex.str() + "\r\n";
			file.seekg(response.byte_reading);
			file.read(buffer.data(), chunkSize);
			response.byte_reading += file.gcount();

			std::string content(buffer.begin(), buffer.end());
			res += content + "\r\n";
			if (file.gcount() == length)
			{
				hex.str("");
				chunkSize = 0;
				hex << std::hex << chunkSize;
				// std::cout << "******> {" << res << "}"<< std::endl;
				response.finish_reading = true;
				res += hex.str() + "\r\n" + "\r\n";
				file.close();
			}
			// if (file.gcount() == 0)
			// {
			// file.close();
			// 	return ("finish_reading");
			// }
		}
		return (res);
	}
	return ("404");
}

std::string read_File_error(std::string Path)
{
	std::ifstream file(Path);
	std::stringstream buffer;

	if (!file)
		return ("not found");
	buffer << file.rdbuf();
	return buffer.str();
}

std::string type_repo(std::string path)
{
	struct stat info;

	std::cout << "response.path_file = " << path << std::endl;
	// std::cout << "*(path.end() - 1) = " << *(path.end() - 1) << std::endl;
	if (*(path.end() - 1) == '/')
		return ("is_directory");
	if (!stat(path.c_str(), &info))
	{
		if (S_ISREG(info.st_mode))
			return ("is_file");
		if (S_ISDIR(info.st_mode))
			return ("is_directory");
	}
	return ("not found");
}
// int main() {
//     std::ofstream file("links.html"); // Open the file for writing

//     if (!file) {
//         std::cerr << "Error creating the file." << std::endl;
//         return 1;
//     }

//     file << "<html>\n";
//     file << "<body>\n";
//     file << "<ol>\n";
//     file << "<li><a href=\"http://www.example.com\">Example Website</a></li>\n";
//     file << "<li><a href=\"http://www.google.com\">Google</a></li>\n";
//     file << "<li><a href=\"http://www.openai.com\">OpenAI</a></li>\n";
//     file << "</ol>\n";
//     file << "</body>\n";
//     file << "</html>\n";

//     file.close(); // Close the file

//     std::cout << "File created successfully." << std::endl;

//     return 0;
// }

std::string content_dir(std::string dir,HttpResponse& response)
{
	DIR* directory = opendir(dir.c_str());

	if (directory)
	{
		struct dirent* content_dir;
		std::ofstream file("content_dir.html");
		if (file)
		{
			file << "<!DOCTYPE html>\n";
			file << "<html>\n";
			file << "<head>\n";
			file << "<title>autoindex</title>\n";
			file << "</head>\n";
			file << "<body>\n";
			file << "<h1>" << "Index of " << response.request.url <<"</h1>\n";
			file << "<ul>\n";
			while ((content_dir = readdir(directory)))
			{
				if (strcmp(content_dir->d_name, ".") && strcmp(content_dir->d_name , "..")
						&& strcmp(content_dir->d_name, ".DS_Store"))
					file << "<li><a href=\"" << "http://"<< response.server_it->ip << ":" << response.server_it->port << response.request.url << '/' << content_dir->d_name << "\">" << content_dir->d_name << "</a></li>\n";
				// content.push_back(content_dir->d_name);
			}
			file << "</ul>\n";
			file << "</body>\n";
			file << "</html>\n";
		}
		file.close();
		closedir(directory);
		return("found");
	}
	return ("not found");
}

void	ft_send_error(int status_code, Config config, HttpResponse& response)
{
	std::string		response_buffer;

	response_Http_Request_error(status_code, config, response);
	response_buffer = generate_http_response(response);
	response_buffer += response.content;
	send(response.fd, response_buffer.c_str(), response_buffer.length(), 0);
}

void init_response(Config& config, HttpResponse& response, HttpRequest& request, int fd)
{
	response.fd = fd;
	response.byte_reading = 0;
	response.request = request;
	response.get_length = false;
	response.finish_reading = false;
	response.server_it = server(config, response.request);
	response.location_it = location(config, response.request, response.server_it);
}

void fill_response(int status_code, HttpResponse& response)
{
	response.version = response.request.version;
	response.code = status_code;
	response.reason_phrase = get_reason_phase(status_code);
	response.headers["Connection"] = "keep-alive";
	response.headers["Content-Type"] = get_content_type(response.path_file);
}

int get_path(Config config, HttpResponse& response)
{
	std::string target = response.location_it->target;
	std::string dir = response.location_it->dir;
	std::string url = response.request.url;
	std::string root = response.server_it->root;
	std::cout << "*********************** dir = " << response.location_it->dir << std::endl;
	std::cout << "*********************** target = " << response.location_it->target << std::endl;
	std::cout << "*********************** url = " << response.request.url << std::endl;
	std::cout << "*********************** " << response.request.url.substr(response.location_it->target.length() - 1, response.request.url.length()) << std::endl;

	size_t	find = url.find(target);
	if (!dir.empty())
	{
		response.path_file = url.substr(0, find) + dir + url.substr(find + target.length(), url.length());
		return (1);
	}
	if (!response.server_it->root.empty())
	{
		response.path_file = url.substr(0, find) + root + url.substr(find + target.length(), url.length());
		return (1);
	}
	ft_send_error(404, config, response);
	return (0);
	// std::cout << "**********response.path_file = " << response.path_file << std::endl;
}

std::string	get_reason_phase(int status_code)
{
	std::map<int, std::string> reason_phase;

	reason_phase[301] = "Moved Permanently"; 
	reason_phase[302] = "Found"; 
	reason_phase[400] = "Bad Request";
	reason_phase[403] = "403 Forbidden";
	reason_phase[404] = "Not Found";
	reason_phase[405] = "Method Not Allowed";
	reason_phase[413] = "Request Entity Too Large";
	reason_phase[414] = "Request-URI Too Long";
	reason_phase[501] = "not implemented";
	reason_phase[200] = "ok";

	return(reason_phase[status_code]);
}

std::string	ft_tostring(int nbr)
{
	std::string	str;

	while (nbr > 10)
	{
		str.insert(0,1, static_cast<char>((nbr % 10) + '0'));
		nbr /= 10;
	}
	if (nbr > 0)
		str.insert(0,1, static_cast<char>(nbr+ '0'));
	return (str);
}

int	response_redirect(HttpResponse& response, Config& config)
{
	std::string type_rep;
	std::string	response_buffer;
	// std::cout <<"response.location_it->creturn.code = " << response.location_it->creturn.code << std::endl;
	// std::cout <<"response.location_it->creturn.to = " << response.location_it->creturn.to << std::endl;
	std::cout << "!!!!!!!!!!!!!!!!!!!!!" << std::endl;
	response.path_file = response.location_it->creturn.to;
	type_rep = type_repo(response.path_file);
	std::cout << "response.path_file = "<< response.path_file << std::endl;
	std::cout << "type_repo = "<< type_rep << std::endl;
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
		std::cout << "******> {" << response_buffer << "}"<< std::endl;
		send(response.fd, response_buffer.c_str(), response_buffer.size(), 0);
	};
	return (0);
}
