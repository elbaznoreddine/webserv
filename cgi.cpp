#include "headers/Server.hpp"
#include "sys/wait.h"
#include "Request.hpp"
#include "cgi.hpp"
#define TIME_OUT 5



std::string getInterpreter(std::string path, LocationConfig &cigLocation)
{
	std::map<std::string, std::string> cgi = cigLocation.getCgiHandlers();
    if ((path.rfind(".py")  == path.size() - 3) && (cgi.find(".py") != cgi.end()))
		return "/usr/bin/python3";
    if ((path.rfind(".php") == path.size() - 4) && (cgi.find(".php") != cgi.end()))
		return "/usr/bin/php";
    return "";
}

bool isCgi(const std::string &path)
{
    return path.find("/cgi-bin/") == 0;
}

char **buildEnv(Request &req)
{
    std::string scriptName = req.path;
    size_t qpos = scriptName.find('?');
    if (qpos != std::string::npos)
        scriptName = scriptName.substr(0, qpos);

    std::vector<std::string> env;
    env.push_back("REQUEST_METHOD="   + req.method);
    env.push_back("CONTENT_LENGTH="   + req.map["Content-Length"]);
    env.push_back("CONTENT_TYPE="     + req.map["Content-Type"]);
    env.push_back("QUERY_STRING="     + req.map["Query-String"]);
    env.push_back("SCRIPT_NAME="      + scriptName);
    env.push_back("SCRIPT_FILENAME=." + scriptName);
    env.push_back("SERVER_PROTOCOL="  + req.protocol);
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("REDIRECT_STATUS=200");
    env.push_back("REQUEST_URI="      + req.path);
    env.push_back("DOCUMENT_ROOT=.");

    char **envp = new char*[env.size() + 1];
    for (size_t i = 0; i < env.size(); i++)
        envp[i] = strdup(env[i].c_str());
    envp[env.size()] = NULL;
    return envp;
}

void freeEnv(char **envp)
{
    for (int i = 0; envp[i]; i++)
        free(envp[i]);
    delete[] envp;
}

void debugCgi(const std::string &full_path, char **env,
              const std::string &interpreter, const std::string &post_body)
{
    std::cerr << "=== SUUUUUUUUUUUU ===" << std::endl;
    std::cerr << "interpreter : " << interpreter    << std::endl;
    std::cerr << "script path : " << full_path      << std::endl;
    std::cerr << "post body   : [" << post_body << "]" << std::endl;
    std::cerr << "env vars    :" << std::endl;
    for (int i = 0; env[i]; i++)
        std::cerr << "  " << env[i] << std::endl;
    std::cerr << "========A FRANI=======" << std::endl;
}

// static std::string makeErrorResponse(int httpStatus, std::string title, std::string detail)
// {
//     std::string body =
//         "<html><head><title>" + title + "</title></head>"
//         "<body><h1>" + title + "</h1><p>" + detail + "</p></body></html>";
// 	std::stringstream ss;
// 	ss << httpStatus;
// 	std::stringstream s;
// 	s << body.size();
//     std::string response =
//         "HTTP/1.1 " + ss.str() + " " + title + "\r\n"
//         "Content-Type: text/html\r\n"
//         "Content-Length: " + s.str() + "\r\n"
//         "Connection: close\r\n\r\n" + body;

//     return response;
// }

static std::string cgiErrorResponse(int cgiErr, int exitCode = 0)
{
	std::stringstream ss;
    switch (cgiErr)
    {
        case CGI_TIMEOUT:
            std::cerr << "[CGI] Error: script timed out (>" << TIME_OUT << "s)\n";
            // 504 Gateway Timeout
			return "";

        case CGI_EXECVE_FAIL:
            std::cerr << "[CGI] Error: execve() failed – bad interpreter or script path\n";
            //  500 Internal Server Error
			return "";

        case CGI_FORK_FAIL:
            std::cerr << "[CGI] Error: fork() failed – " << strerror(errno) << "\n";
            // 500 Internal Server Error
			return "";

        case CGI_PIPE_FAIL:
            std::cerr << "[CGI] Error: pipe() failed – " << strerror(errno) << "\n";
            // 500 Internal Server Error
			return "";

        case CGI_EXIT_ERROR:
			ss << exitCode;
            std::cerr << "[CGI] Error: script exited with code " << exitCode << "\n";
            // 500 Internal Server Error
			return "";

        default:
            //500 Internal Server Error Unknown CGI error
			return "";
    }
}

std::string cgiHandler(std::string full_path, char **env,
                       std::string interpreter, std::string post_body)
{
    debugCgi(full_path, env, interpreter, post_body);

    int stdin_pipe[2];
    int stdout_pipe[2];

    if (pipe(stdin_pipe) < 0 || pipe(stdout_pipe) < 0)
        return cgiErrorResponse(CGI_PIPE_FAIL);

    pid_t pid = fork();
    if (pid < 0)
    {
        close(stdin_pipe[0]);
		close(stdin_pipe[1]);
        close(stdout_pipe[0]);
		close(stdout_pipe[1]);
        return cgiErrorResponse(CGI_FORK_FAIL);
    }

    if (pid == 0)
    {
        dup2(stdin_pipe[0],  STDIN_FILENO);
        dup2(stdout_pipe[1], STDOUT_FILENO);
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);

        char *args[] = {
            (char *)interpreter.c_str(),
            (char *)full_path.c_str(),
            NULL
        };
        if (execve(interpreter.c_str(), args, env) < 0)
			cgiErrorResponse(CGI_EXECVE_FAIL);
    	exit(1);
    }

    close(stdin_pipe[0]);
    close(stdout_pipe[1]);

    if (!post_body.empty())
    {
        const char *data = post_body.c_str();
        int size = (int)post_body.size();
        int sent = 0;
        fcntl(stdin_pipe[1], F_SETFL, O_NONBLOCK);
        while (sent < size)
        {
            int ret = write(stdin_pipe[1], data + sent, size - sent);
            if (ret < 0)
				break;
			if (ret == 0)
				continue;
            sent += ret;
        }
    }
    close(stdin_pipe[1]);

    fcntl(stdout_pipe[0], F_SETFL, O_NONBLOCK);
    time_t start_time = time(NULL);
    bool   timed_out  = false;
    std::string out;
    char buf[4096];
    int  status = 0;

    while (true)
    {
        int check = waitpid(pid, &status, WNOHANG);
        if (check == pid)
            break;

        int n = read(stdout_pipe[0], buf, sizeof(buf));
        if (n > 0)
            out.append(buf, n);

        if (time(NULL) - start_time >= TIME_OUT)
        {
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0);
            timed_out = true;
            break;
        }
        usleep(1000);
    }

    int n;
    while ((n = read(stdout_pipe[0], buf, sizeof(buf))) > 0)
        out.append(buf, n);
    close(stdout_pipe[0]);

    if (timed_out)
        return cgiErrorResponse(CGI_TIMEOUT);

    if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        return cgiErrorResponse(CGI_EXIT_ERROR, WEXITSTATUS(status));

    return out;
}