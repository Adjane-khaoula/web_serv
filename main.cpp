#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#ifdef __APPLE__
#include <sys/event.h>
#elif __linux__
#include <sys/epoll.h>
#endif
#include "webserv.hpp"

void die(std::string msg) {
	perror(msg.c_str());
	exit(1);
}

Config config;
int max_server_fd;
/**
 * spawn servers and add their sockets to watchlist
 * wait for events (connections, requests) and handle them serially
 * 
*/

void spawn_servers(int wfd) {
	for (size_t i = 0; i < config.servers.size(); i++) {
		int sock;
		assert((sock = socket(PF_INET, SOCK_STREAM, 0)) != -1);

		int enable = 1;
		assert(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == 0);

		struct sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr(config.servers[i].ip.c_str());
		address.sin_port = htons(config.servers[i].port);

		assert(bind(sock, (struct sockaddr *)&address, sizeof(address)) == 0);

		assert(listen(sock, BACKLOG_SIZE) == 0);
		std::cout << "--------- " << "listening on: " << config.servers[i].ip << ":" << config.servers[i].port << std::endl;

#ifdef __APPLE__
		watchlist_add_fd(wfd, sock, EVFILT_READ);
#elif __linux__
		watchlist_add_fd(wfd, sock, EPOLLIN);
#endif
		max_server_fd = sock;
	}
}

void accept_connection(int wfd, int server) {
	struct sockaddr_in caddress;
	socklen_t len = sizeof caddress;
	int client;
	assert((client = accept(server, (struct sockaddr *)&caddress, &len)) != -1);
	
#ifdef __APPLE__
		watchlist_add_fd(wfd, client, EVFILT_READ);
#elif __linux__
		watchlist_add_fd(wfd, client, EPOLLIN);
#endif

	std::cout << "--------- " << "connection received " << inet_ntoa(caddress.sin_addr) << ":" << ntohs(caddress.sin_port) << std::endl;
}
////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
	if (argc != 2)
		die("usage: webserv <config_file>\n");

	parse_config(argv[1]);
	// dump_config(config);

    int wfd = init_watchlist();
	spawn_servers(wfd);

	int ret;

	while (1) {
		int fd = watchlist_wait_fd(wfd);

		if (fd <= max_server_fd) {
			accept_connection(wfd, fd);
			continue;
		}

		std::string request_buffer;
		std::string response_buffer;
		HttpRequest request;
		// HttpResponse response;
		int status_code;
		std::map<int,HttpResponse> responses;
		HttpResponse	response;
		std::string res_str;
		std::map<int, HttpResponse>::iterator it = responses.find(fd);

		while (1) {
			// std::cout << "/////////////// \n";
			char buffer[255];
			if ((ret = recv(fd, buffer, sizeof buffer, 0)) < 0)
				goto close_socket;
			buffer[ret] = 0;
			request_buffer += buffer;
			if (ret != sizeof buffer)
				break;
		}

		if (request_buffer.length() == 0)
			goto close_socket;
		else
			std::cout << "--------- request received"<< std::endl;
		// HttpRequest req;

		parse_http_request(request_buffer, request);

		if (responses.empty() || it == responses.end())
		{
			// InfoClient		info_client;
			responses[fd] = response;
			// response.clients[fd] = info_client;
			responses[fd].request = request;
			responses[fd].it = server(config, responses[fd].request);
			responses[fd].it2 = location(config, responses[fd].request, responses[fd].it);
			status_code = check_req_well_formed(fd, config, responses);
			if (status_code == 1)
			{
				// std::cout << "************> status_code == " << status_code<< std::endl;
				response_get(fd, config, responses);
				responses[fd].content = read_File(responses, fd);
				res_str = generate_http_response(responses[fd]);
				res_str += responses[fd].content;
				// std::cout << "*********** {" << res_str << "}" << std::endl;
				send(fd, res_str.c_str(), res_str.length(), 0) ;
			}
			else
				response_Http_Request_error(status_code, request, config, response);
		}
		else
		{
			responses[fd].content = read_File(responses, fd);
			res_str = generate_http_response(responses[fd]);
			res_str += responses[fd].content;
			// std::cout << "*********** {" << res_str << "}" << std::endl;
			send(fd, res_str.c_str(), res_str.length(), 0) ;
		}
			//else if (status_code == 2)
			// 	response_post(req, config, response);
			//else if (status_code == 3)
			// 	response_delete(req, config, response);
		continue;
close_socket:
			std::cout << "--------- invalid request: close socket"<< std::endl;
			watchlist_del_fd(wfd, fd);
			close(fd);
			continue;
	}
}

		// std::cout << "\033[32m"  << "method: " << responses[fd].request.method<< "\033[0m" << std::endl;
		// std::cout << "\033[32m"  << "url: " << responses[fd].request.url<< "\033[0m" << std::endl;
		// std::cout << "\033[32m"  << "version: " << responses[fd].request.version << "\033[0m" << std::endl;

		// for (auto it = responses[fd].request.headers.begin(); it != responses[fd].request.headers.end(); it++) {
		// 	std::cout << "\033[32m" << it->first << ' ' << it->second << "\033[0m" << std::endl;
		// }

// method: GET
// url: /
// version: HTTP/1.1
// Accept  */*
// Accept-Encoding  gzip, deflate, br
// Connection  keep-alive
// Content-Length  10
// Content-Type  text/plain
// Host  0.0.0.0:8080
// Postman-Token  3f6de512-08ad-4c0b-932c-b8007843f934
// Transfer-Encoding chunked





//add time out




////////////////////////////////////////////////////////////////////////////////////////////////////
		// response.it = server(config, req);
		// 	response.it2 = location(config, req, response.it);
		// 	int	status_code = check_req_well_formed(req, config, response);
		// 	if (status_code == 1)
		// 	{
		// 		// std::cout << "************> status_code == " << status_code<< std::endl;
		// 		response_get(req, config, response);
		// 	}
		// 	else
		// 		response_Http_Request_error(status_code, req, config, response);
		// 	std::string res_str = generate_http_response(response);
		// 	send(fd, res_str.c_str(), res_str.length(), 0) ;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////






// #include <iostream>
// #include <fstream>
// #include <string>

// void readFile(const std::string& filename, std::streampos startPos, std::streampos endPos) {
//     std::ifstream file(filename);
//     std::string line;
//     file.seekg(startPos);  // Set the file pointer to the starting position

//     while (file.tellg() <= endPos && std::getline(file, line)) {
//         // Process the line of the file
//         std::cout << "Reading " << filename << ": " << line << std::endl;
//     }

//     file.close();
// }

// int main() {
//     std::string file1 = "file1.txt";
//     std::string file2 = "file2.txt";

//     std::ifstream file1Stream(file1, std::ios::binary);
//     std::ifstream file2Stream(file2, std::ios::binary);

//     // Get the file sizes
//     file1Stream.seekg(0, std::ios::end);
//     file2Stream.seekg(0, std::ios::end);
//     std::streampos file1Size = file1Stream.tellg();
//     std::streampos file2Size = file2Stream.tellg();

//     // Calculate the positions for splitting the files
//     std::streampos file1Mid = file1Size / 2;
//     std::streampos file2Mid = file2Size / 2;

//     // Read the first halves of the files
//     readFile(file1, 0, file1Mid);
//     readFile(file2, 0, file2Mid);

//     // Read the second halves of the files
//     readFile(file1, file1Mid, file1Size);
//     readFile(file2, file2Mid, file2Size);

//     return 0;
// }
