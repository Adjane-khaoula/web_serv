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

std::string read_File(std::string& Path) {
	std::ifstream file(Path);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

	int	check_req_well_formed(HttpRequest &req)
	{
		std::map<std::string, std::string>::iterator it;
		std::string client_max_body_size = "1M";//the maximum size limit in megabytes (MB) for the request body. 
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
		// std::cout << "------------" << ft_atof(client_max_body) << std::endl;
		// std::cout << "*************"<< 1024 * 1024 << std::endl;
		// std::cout << "*************"<< ft_atof(req.headers["Content-Length"]) << std::endl;
		if (ft_atof(req.headers["Content-Length"]) > ft_atof(client_max_body) * 1024 * 1024)
			return (413); //413 Request Entity Too Large
		return(200);
	}

	// int	check_location()
	// {

	// }

	std::string generate_http_response(HttpResponse &res) {
	std::stringstream res_str;

    time_t t = time(NULL);
    char mbstr[100];
 
    strftime(mbstr, sizeof(mbstr), "%a, %d %b %Y %T GMT", localtime(&t));

	res.headers["Server"] = "ft_webserv/0.1";
	res.headers["Date"] = mbstr;
	res.headers["Connection"] = "keep-alive";
	res.headers["Content-Length"] = std::to_string(res.content.length());

	// Last-Modified: Tue, 28 Mar 2023 15:01:54 GMT
	// ETag: "64230162-267"

	res_str << res.version << " " << res.code << res.reason_phrase << HTTP_DEL;
	for (std::map<std::string, std::string>::iterator it = res.headers.begin(); it != res.headers.end(); it++)
		res_str << it->first << ": " << it->second << HTTP_DEL;

	res_str << HTTP_DEL;
	res_str << res.content;
	return res_str.str();
}

	HttpResponse response_Http_Request(int status_code, HttpRequest& request) {
		HttpResponse response;
		// Set the HTTP version
		response.version = request.version;
		// Process the request and generate the appropriate response
		// Here, you can implement your business logic based on the request's method, URL, etc.	
		// Set the status code, reason phrase, and content
		response.code = status_code;
		switch(status_code) {
			case 501 :
				response.reason_phrase = "not implemented";
				response.content = read_File("www/501.html");
				response.headers["Content-Type"] = "text/html";
				break ;
			case 400:
				response.reason_phrase = "Bad Request";
				response.content = read_File("www/400.html");
				response.headers["Content-Type"] = "text/html";
				break ;
			case 414:
				response.reason_phrase = "Request-URI Too Long";
				response.content = read_File("www/414.html");
				response.headers["Content-Type"] = "text/html";
				break ;
			case 413:
				response.reason_phrase = "Request Entity Too Large";
				response.content = read_File("www/413.html");
				response.headers["Content-Type"] = "text/html";
				break ;
			case 200:
				response.reason_phrase = "ok";
				response.content = "ara chi boysa";
				response.headers["Content-Type"] = "text/html";
				break ;
		}
		// response.content = request.content;
		// response.content = "Hello, World!";
		// Set any desired headers
		response.headers["Content-Length"] = std::to_string(response.content.length());
		// response.headers["Content-Type"] = "text/html";
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



// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <map>
// #include <boost/asio.hpp>

// class HttpResponse {
// public:
//     std::string version;
//     int status;
//     std::string statusMessage;
//     std::map<std::string, std::string> headers;
//     std::string content;
// };

// std::string readFileContents(const std::string& filePath) {
//     std::ifstream file(filePath);
//     std::stringstream buffer;
//     buffer << file.rdbuf();
//     return buffer.str();
// }

// std::string generateErrorResponse(const std::string& errorCode, const std::string& statusMessage) {
//     std::string errorTemplate = R"(
//         <html>
//         <head>
//             <title>Error {errorCode}</title>
//         </head>
//         <body>
//             <h1>Error {errorCode}</h1>
//             <p>{statusMessage}</p>
//         </body>
//         </html>
//     )";

//     // Replace placeholders with actual values
//     std::string errorResponse = errorTemplate;
//     size_t errorCodePos = errorResponse.find("{errorCode}");
//     if (errorCodePos != std::string::npos) {
//         errorResponse.replace(errorCodePos, 11, errorCode);
//     }

//     size_t statusMessagePos = errorResponse.find("{statusMessage}");
//     if (statusMessagePos != std::string::npos) {
//         errorResponse.replace(statusMessagePos, 14, statusMessage);
//     }

//     return errorResponse;
// }

// void sendResponse(boost::asio::ip::tcp::socket& socket, const HttpResponse& response) {
//     std::ostringstream oss;
//     oss << response.version << " " << response.status << " " << response.statusMessage << "\r\n";

//     for (const auto& header : response.headers) {
//         oss << header.first << ": " << header.second << "\r\n";
//     }

//     oss << "\r\n" << response.content;

//     std::string responseStr = oss.str();

//     boost::system::error_code error;
//     boost::asio::write(socket, boost::asio::buffer(responseStr), error);

//     if (error) {
//         std::cerr << "Error sending response: " << error.message() << std::endl;
//     }
// }

// void handleRequest(const HttpRequest& request, boost::asio::ip::tcp::socket& socket) {
//     HttpResponse response;

//     // Set response headers
//     response.version = "HTTP/1.1";
//     response.status = 404;
//     response.statusMessage = "Not Found";
//     response.headers["Content-Type"] = "text/html";

//     // Read the error HTML file contents
//     std::string errorHtml = readFileContents("error404.html");

//     // Generate the error response content
//     response.content = generateErrorResponse("404", "Page not found");

//     sendResponse(socket, response);
// }

// int main() {
//     // Initialize the Boost.Asio library
//     boost::asio::io_context ioContext;

//     // Create a TCP acceptor to listen for incoming connections
//     boost::asio::ip::tcp::acceptor acceptor(ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8080));

//     while (true) {
//         // Wait for a client to connect
//         boost::asio::ip::tcp::socket socket(ioContext);
//         acceptor.accept(socket);

//         // Read the HTTP request from the client
//         // Implement the logic to populate the `HttpRequest` struct

//         HttpRequest request;
//         // Populate the request with the necessary data

//         // Handle the request and send the response
//         handleRequest(request, socket);

//         // Close the socket
//         socket.close();
//     }

//     return 0;
// }
