#include <sys/wait.h>
#include "../webserv.hpp"
#include "../config.hpp"
#include <fcntl.h>
#include <cstring>
#include <cstdio>

char** get_env(HttpResponse& response)
{
    std::vector<std::string> vect;
    (void) response;
    std::string content_length = ft_tostring(response.request.content.size());
    std::cout << "@@@@@@@@@@@@@@@@@@@@@@ " << content_length << std::endl;
    std::cout << "@@@@@@@@@@@@@@@@@@@@@@ " << response.request.method << std::endl;
    std::cout << "@@@@@@@@@@@@@@@@@@@@@@ " << response.path_file << std::endl;
    vect.push_back("REQUEST_METHOD=" + response.request.method);
    vect.push_back("SCRIPT_FILENAME=" + response.path_file);
    vect.push_back("PATH_INFO=" + response.path_file);
    vect.push_back("CONTENT_LENGTH="  + content_length);
    vect.push_back("REDIRECT_STATUS=200");
    vect.push_back("GATEWAY_INTERFACE=CGI/1.1");
    vect.push_back("QUERY_STRING=" + response.query_str);
    // vect.push_back("CONTENT_TYPE=application/x-www-form-urle÷ncoded");
    std::cout << "++++++++++++++++++++++++++++> {" << response.request.headers["Content-Type"] << "}" << std::endl;
    vect.push_back("CONTENT_TYPE=" + response.request.headers["Content-Type"]);
    // vect.push_back("REQUEST_url=/Users/kadjane/Desktop/web_serv2/test.php"); 
    // vect.push_back("CONTENT_TYPE=text/html");


    // vect.push_back("REQUEST_url=/Users/kadjane/Desktop/web_serv2/test.php"); 
    // vect.push_back("SERVER_PORT=8080");  
    // vect.push_back("HTTP_HOST=0.0.0.0"); 
    // vect.push_back("SERVER_PROTOCOL=HTTP/1.1");
    // vect.push_back("SCRIPT_NAME=/Users/kadjane/Desktop/web_serv2/test.php");

    char** env = new char*[vect.size() + 1];
    size_t i;
    for (i = 0; i < vect.size(); ++i)
    {
        if (response.request.method == "POST")
            std::cout << SKY << "{"<< vect[i] << "}"<< END << std::endl;
        env[i] = new char[vect[i].length() + 1];
        strcpy(env[i], vect[i].c_str());
    }
    env[vect.size()] = NULL; 
    return env;
}

void cgi_response_content(HttpResponse & response)
{
    std::ifstream out_file("output.txt", std::ifstream::binary);
    std::ofstream file("out");
    std::string line;

    while(std::getline(out_file, line) && line != "\r")
    {
        if (line.find("Content-type") != std::string::npos)
        {
            int length = line.find(";") - (line.find(" ") + 1);
            response.headers["Content-Type"] = line.substr(line.find(" ") + 1, length);
        }
    }
    while (std::getline(out_file, line))
    {
        line += "\n";
        file << line;
    }
    response.path_file = "out";

}
// /usr/bin/python
void    execute_cgi(HttpResponse &response)
{
    pid_t pid = fork();
    CGI cgi;
    int output_fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    std::ofstream content("cgi.txt");

    if (pid == 0)
    {
        std::string path = response.path_file;
        std::string path_executable = response.location_it->cgi[0].cgi_pass;
        std::string str(response.request.content.begin(), response.request.content.end());
        // std::cout << "\033[33m" << "{" << str << "}" << "\033[00m"  << std::endl;
        if (!response.request.content.empty())
        {
            // std::cout << "\033[32m" << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << "\033[0m" << std::endl;
            // std::cout << "\033[34m content == {" << str <<  "} \033[00m" << std::endl; 
            content << str;
            content.close();
        }
        int input_fd = open("cgi.txt", O_RDONLY , 0666);
        // std::cout << "################################ "<< input_fd << std::endl;
        char **env;
        env = get_env(response);

        std::cerr << PURPLE << path_executable << END << std::endl;
        std::cerr << PURPLE  << "path == " << path << END << std::endl;
        char* const argv[] = {(char*)path_executable.c_str(), (char*)path.c_str(), NULL};
        if (dup2(output_fd, 1) == -1) {
            std::cout << "Error duplicating file descriptor." << std::endl;
            return ;
        }
        // dup2(output_fd, 2) == -1
        if (dup2(input_fd, STDIN_FILENO) < 0) {
            std::cerr << "Error duplicating input file descriptor." << std::endl;
            close(input_fd);
            return;
        }
        // close(input_fd);

        // Read the content from STDIN
        std::string stdinContent;
        std::ofstream cin("cin_file");
        while(std::getline(std::cin, stdinContent))
        {
            cin << stdinContent;
        }
        cin.close(); 
    //    if (execve(argv[0], argv, env) < 0){
    //         std::cerr << RED << "Error executing CGI script."<< END << std::endl;
    //         return ;
    //     }
    }
    else
    {
        waitpid(pid, NULL, 0);
        cgi_response_content(response);
        // content.close();
        // close(input_fd);
        close(output_fd);
    }
}


        // std::ifstream input("cgi.txt");
        // std::string line;
        // if (std::getline(input, line)) {
        // std::cout << "Line: " << line << std::endl;
        // } else {
        //     std::cout << "File is empty." << std::endl;
        // }    
        // input.close();

// void parse_query_string(HttpResponse &response)
// {
// 	std::string query_str_parsed;
// 	std::vector<std::string> space;
// 	space.push_back("%20");
// 	space.push_back("%2B");
// 	space.push_back("+");
// 	int find = true;

//     query_str_parsed += response.query_str;
// 	while (find)
// 	{
// 		find = false;
// 		for (std::vector<std::string>::iterator space_it = space.begin(); space_it != space.end(); space_it++)
// 		{
// 			if (query_str_parsed.find(*space_it) != std::string::npos)
// 			{
// 				query_str_parsed = query_str_parsed.substr(0,query_str_parsed.find(*space_it)) + " "
//                     + query_str_parsed.substr(query_str_parsed.find(*space_it) + (*space_it).length(), query_str_parsed.length());
//                 std::cout << "\033[33m" << "query_parsed == {" << query_str_parsed << "}" << "\033[00m" << std::endl;
// 				find = true;
//                 break ;
// 			}
// 		}
// 	}
// 	std::cout << "\033[32m" << "*******> {" << query_str_parsed << "\033[0m" << std::endl;
// }