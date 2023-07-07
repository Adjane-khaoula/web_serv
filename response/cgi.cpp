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
    vect.push_back("REQUEST_METHOD=" + response.request.method);
    vect.push_back("SCRIPT_FILENAME=" + response.path_file);
    vect.push_back("PATH_INFO=" + response.path_file);
    vect.push_back("CONTENT_LENGTH=0"  + response.headers["Content-Length"]);
    vect.push_back("REDIRECT_STATUS=200");
    vect.push_back("GATEWAY_INTERFACE=CGI/1.1");
    vect.push_back("QUERY_STRING=name=khawla");
    // vect.push_back("REQUEST_url=/Users/kadjane/Desktop/web_serv2/test.php"); 
    // vect.push_back("CONTENT_TYPE=text/html");


    // vect.push_back("REQUEST_METHOD=GET");// + response.request.method);
    // vect.push_back("SCRIPT_FILENAME=/Users/kadjane/Desktop/server_1copy/response/cgi-bin/test.php");// + response.path_file);
    // vect.push_back("PATH_INFO=/Users/kadjane/Desktop/server_1copy/response/cgi-bin/test.php");
    // vect.push_back("CONTENT_LENGTH=0");// + response.headers["Content-Length"]);
    // vect.push_back("REDIRECT_STATUS=200");
    // vect.push_back("GATEWAY_INTERFACE=CGI/1.1");
    // vect.push_back("REQUEST_url=/Users/kadjane/Desktop/web_serv2/test.php"); 
    // vect.push_back("CONTENT_TYPE=text/html");
    // vect.push_back("SERVER_PORT=8080");  
    // vect.push_back("HTTP_HOST=0.0.0.0"); 
    // vect.push_back("SERVER_PROTOCOL=HTTP/1.1");
    // vect.push_back("SCRIPT_NAME=/Users/kadjane/Desktop/web_serv2/test.php");
    // vect.push_back(NULL);

    char** env = new char*[vect.size() + 1];
    size_t i;
    for (i = 0; i < vect.size(); ++i)
    {
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

void    execute_cgi(HttpResponse &response)
{
    pid_t pid = fork();
    CGI cgi;
    int output_fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

    std::string path = response.path_file;
    std::string path_executable = response.location_it->cgi[0].cgi_pass;

    if (pid == 0)
    {
        char **env;
        env = get_env(response);
        char* const argv[] = {(char*)path_executable.c_str(), (char*)path.c_str(), NULL};
        if (dup2(output_fd, 1) == -1 ) {
            std::cout << "Error duplicating file descriptor." << std::endl;
            return ;
        }

        if (dup2(output_fd, 2) == -1){
            std::cout << "Error duplicating file descriptor." << std::endl;
            return ;
        }

        if (execve(argv[0], argv, env) == -1){
            std::cout << "Error executing CGI script." << std::endl;
            return ;
        }

    }
    else
    {
        waitpid(pid, NULL, 0);
        cgi_response_content(response);
        close(output_fd);
    }
}
