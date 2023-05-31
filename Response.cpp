	#include "webserv.hpp"

	int	check_req_well_formed(HttpRequest &req)
	{
		std::map<std::string, std::string>::iterator it;

		it = req.headers.find("Transfer-Encoding");
		if (it != req.headers.end() && it->second.erase(0, 1) != "chunked")
			return (501);//not implemented
		if (it == req.headers.end()
			&& req.headers.find("Content-Length") == req.headers.end() && req.method == "POST")
			return(400);//Bad Request
		// std::cout << "{" << req.url << "}" << std::endl;
		if (req.url.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%")
				!= std::string::npos)
			return(40);//Bad Request
		if (req.url.length() > 2048)
			return (414);//Request-URI Too Long
		// if (req.headers["Content-Length"] > )
		return(10);
	}
	
	HttpResponse processHttpRequest(const HttpRequest& request) {
		HttpResponse response;
		// Set the HTTP version
		response.version = request.version;
		// Process the request and generate the appropriate response
		// Here, you can implement your business logic based on the request's method, URL, etc.	
		// Set the status code, reason phrase, and content
		response.code = 200;
		response.reason_phrase = "OK";
		response.content = request.content;
		// response.content = "Hello, World!";
		// Set any desired headers
		response.headers["Content-Length"] = std::to_string(response.content.length());
		response.headers["Content-Type"] = "text/html";
		return response;
	}


// ********** method: GET
// ********** url: /
// ********** version: HTTP/1.1
//request headers
// --------- Accept  text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
// --------- Accept-Encoding  gzip, deflate, br
// --------- Accept-Language  en-US,en;q=0.9,fr;q=0.8
// --------- Cache-Control  max-age=0
// --------- Connection  keep-alive
// --------- Host  127.0.0.1:8080
// --------- Upgrade-Insecu∫e-Requests  1
// --------- User-Agent  Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/113.0.0.0 Safari/537.36
// --------- sec-ch-ua  "Google Chrome";v="113", "Chromium";v="113", "Not-A.Brand";v="24"
// --------- sec-ch-ua-mobile  ?0
// --------- sec-ch-ua-platform  "macOS"
// --------- connection received 127.0.0.1:51922