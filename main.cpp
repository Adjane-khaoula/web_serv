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

void parse_config(std::string config_file) {
	// TODO:
	config.servers.resize(2);
	config.servers[0].ip = "0.0.0.0";
	config.servers[0].port = 8080;

	config.servers[1].ip = "0.0.0.0";
	config.servers[1].port = 8090;
}

int main(int argc, char **argv) {
	assert (argc == 2);

	parse_config(argv[1]);
	HttpResponse response;
    int wfd = init_watchlist();
	spawn_servers(wfd);

	int ret;

	while (1) {
		int fd = watchlist_wait_fd(wfd);

		if (fd <= max_server_fd) {
			accept_connection(wfd, fd);
			continue;
		}

		std::string request;
		while (1) {
			char buffer[255];
			assert((ret = recv(fd, buffer, sizeof buffer, 0)) > 0);
			buffer[ret] = 0;
			request += buffer;
			if (ret != sizeof buffer)
				break;
		}

		HttpRequest req;
		parse_http_request(request, req);
		std::cout << "\033[33m" << check_req_well_formed(req) << "\033[0m" << std::endl;
		// response = processHttpRequest(req);
		// std::cout << "********** " << "response.code " << response.code << std::endl;
		// std::cout << "********** " << "response.reason_phrase " << response.reason_phrase << std::endl;
		// std::cout << "********** " << "response.content " << response.content << std::endl;

		// for (auto it = response.headers.begin(); it != response.headers.end(); it++) {
		// 	std::cout << "--------- " << it->first << ' ' << it->second << std::endl;
		// }
		std::cout << "\033[32m"  << "method: " << req.method<< "\033[0m" << std::endl;
		std::cout << "\033[32m"  << "url: " << req.url<< "\033[0m" << std::endl;
		std::cout << "\033[32m"  << "version: " << req.version << "\033[0m" << std::endl;

		for (auto it = req.headers.begin(); it != req.headers.end(); it++) {
			std::cout << "\033[32m" << it->first << ' ' << it->second << "\033[0m" << std::endl;
		}

		HttpResponse res;
		res.code = 200;
		res.content = "ft_webserv yora7ibo bikom";
		res.headers["Content-Type"] = "text/html";
		std::string res_str = generate_http_response(res);
		assert(send(fd, res_str.c_str(), res_str.length(), 0) == (ssize_t)res_str.length());
	}
}
