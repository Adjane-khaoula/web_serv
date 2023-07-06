// #include <iostream>
// #include <unistd.h>
// #include <iostream>
// #include <unistd.h>
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
    vect.push_back("REQUEST_METHOD=GET");// + response.request.method);
    vect.push_back("SCRIPT_FILENAME=/Users/kadjane/Desktop/server_1copy/response/cgi-bin/test.php");// + response.path_file);
    vect.push_back("CONTENT_LENGTH=0");// + response.headers["Content-Length"]);
    vect.push_back("REDIRECT_STATUS=200");

    // vect.push_back("CONTENT_TYPE=text/html");
    // vect.push_back("SERVER_PORT=8080");  
    // vect.push_back("HTTP_HOST=0.0.0.0"); 
    // vect.push_back("PATH_INFO=/Users/kadjane/Desktop/web_serv2/test.php");  
    // vect.push_back("REQUEST_url=/Users/kadjane/Desktop/web_serv2/test.php"); 
    // vect.push_back("SERVER_PROTOCOL=HTTP/1.1");
    // vect.push_back("SCRIPT_NAME=/Users/kadjane/Desktop/web_serv2/test.php");
    // vect.push_back("GATEWAY_INTERFACE=CGI/1.1");
    // vect.push_back(NULL);

    std::cout << "---------------------" << vect.size() << std::endl;
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

void    execute_cgi(HttpResponse &response)
{
    pid_t pid = fork();
    CGI cgi;

    std::cout << response.path_file << std::endl;
    // std::string path = response.path_file;
    std::string path = "/Users/kadjane/Desktop/server_1copy/response/cgi-bin/test.php";
    std::string path_executable = /*cgi.cgi_pass;*/ "response/cgi-bin/php-cgi";
    // std::string path_executable = response.location_it->cgi[0].cgi_pass;
    // std::cout << "*******> {" << path_executable<<"}" << std::endl;

    
    if (pid == 0)
    {
        char **env;

        env = get_env(response);
        // std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@" << env[0]<< std::endl;
         char* const argv[] = {(char*)path_executable.c_str(), (char*)path.c_str(), NULL};

        int output_fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (dup2(output_fd, 1) == -1) {
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
    }
}



//     response.path_file = "cgi-bin/test.php";
//     char* env[] = {
//        (char *)"CONTENT_LENGTH=0",       // Set any necessary environment variables
//     //    (char*)"CONTENT_TYPE=text/html",
//        (char *)"REQUEST_METHOD=GET",
//     //    (char *)"PATH_INFO=test.php",
//     //    (char *)"SERVER_PROTOCOL=HTTP/1.1",
//        (char *)"SCRIPT_NAME=cgi-bin/php-cgi",
//     //    (char *)"SERVER_PORT=8080",
//     //    (char *)"HTTP_HOST=0.0.0.0",
//     //    (char *)"GATEWAY_INTERFACE=CGI/1.1",
//     //    (char *)"REQUEST_url=test.php",
//        (char *)"REDIRECT_STATUS=true",
//     //    (char *)"QUERY_STRING=",
//     //    (char *)"HTTP_COOKIE=",
//        (char *)(std::string("SCRIPT_FILENAME=") + response.path_file),
//        (char*)nullptr
//    };