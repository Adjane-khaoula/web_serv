#include "webserv.hpp"
#include "config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
// #include <sys/stat.h>

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
	std::map<std::string, std::string> content_type;
	std::string type = req.url.substr(req.url.rfind(".") + 1,req.url.length());

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
	for (std::vector<Location>::iterator it2 = server->routes.begin(); it2 != server->routes.end();it2++)
		if (req.url.find(it2->target) != std::string::npos && req.url.find(it2->target) == 0)
			return (it2);
	return (server->routes.end());
}

std::string read_File(std::map<int,HttpResponse>& clients, int fd )
{
	std::ifstream file(clients[fd].path_file, std::ifstream::binary);

	if (file )
	{
		file.seekg (0, file.end);
		int length = file.tellg();
		file.seekg (0, file.beg);
		std::vector<char> buffer(100);
		if (clients[fd].get_length == false)
		{
			clients[fd].get_length = true;
			return (std::to_string(length));
		}
		if (clients[fd].byte_reading < length) {
		int chunkSize = std::min(BUFF_SIZE, length - clients[fd].byte_reading);
		buffer.resize(chunkSize);
		file.read(buffer.data(), chunkSize);	
		clients[fd].byte_reading += file.gcount();
		if (file.gcount() == 0)
		{
			file.close();
			return ("finish_reading");
		}
		}
		std::string content(buffer.begin(), buffer.end());
		return (content);
	}
	return ("404");
}

std::string read_File_error(std::string Path)
{
	std::ifstream file(Path);
	std::stringstream buffer;


	if (!file)
		return ("not found");
	// std::cout << "*******> " << Path << std::endl;
	buffer << file.rdbuf();
	return buffer.str();
}

std::string type_repo(std::string path)
{
	struct stat info;

	// path = "/Users/kadjane/Desktop/web_serv2/srcs";
	// std::cout << "path == {" << path<< "}" << std::endl;
	if (*(path.end() - 1) == '/')
		return ("is_directory with /");
	if (!stat(path.c_str(), &info))
	{
		// std::cout << "/////////////////////////////" << std::endl;
		if (S_ISREG(info.st_mode))
			return ("is_file");
		if (S_ISDIR(info.st_mode))
			return ("is_directory");
	}
	return ("not found");
}

std::string content_dir(std::string dir, std::vector<std::string>& content)
{
	DIR* directory = opendir(dir.c_str());

	if (directory)
	{
		struct dirent* content_dir;
        while ((content_dir = readdir(directory)))
			content.push_back(content_dir->d_name);
        closedir(directory);
		return("found");
	}
	return ("not found");
	// else
		// std::cout << "/////////////////////////// ma t7alch\n";
}