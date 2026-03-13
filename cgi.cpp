#include "headers/Server.hpp"
#include "sys/wait.h"
#define TIME_OUT 5

std::string path = "cgi-bin/t.py";
std::string post = "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu, consequat vitae, eleifend ac, enim. Aliquam lorem ante, dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla ut metus varius laoreet. Quisque rutrum. Aenean imperdiet. Etiam ultricies nisi vel augue. Curabitur ullamcorper ultricies nisi. Nam eget dui. Etiam rhoncus. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum. Nam quam nunc, blandit vel, luctus pulvinar, hendrerit id, lorem. Maecenas nec odio et ante tincidunt tempus. Donec vitae sapien ut libero venenatis faucibus. Nullam quis ante. Etiam sit amet orci eget eros faucibus tincidunt. Duis leo. Sed fringilla mauris sit amet nibh. Donec sodales sagittis magna. Sed consequat, leo eget bibendum sodales, augue velit cursus nunc, quis gravida magna mi a libero. Fusce vulputate eleifend sapien. Vestibulum purus quam, scelerisque ut, mollis sed, nonummy id, metus. Nullam accumsan lorem in dui. Cras ultricies mi eu turpis hendrerit fringilla. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; In ac dui quis mi consectetuer lacinia. Nam pretium turpis et arcu. Duis arcu tortor, suscipit eget, imperdiet nec, imperdiet iaculis, ipsum. Sed aliquam ultrices mauris. Integer ante arcu, accumsan a, consectetuer eget, posuere ut, mauris. Praesent adipiscing. Phasellus ullamcorper ipsum rutrum nunc. Nunc nonummy metus. Vestibulum volutpat pretium libero. Cras id dui. Aenean ut eros et nisl sagittis vestibulum. Nullam nulla eros, ultricies sit amet, nonummy id, imperdiet feugiat, pede. Sed lectus. Donec mollis hendrerit risus. Phasellus nec sem in justo pellentesque facilisis. Etiam imperdiet imperdiet orci. Nunc nec neque. Phasellus leo dolor, tempus non, auctor et, hendrerit quis, nisi. Curabitur ligula sapien, tincidunt non, euismod vitae, posuere imperdiet, leo. Maecenas malesuada. Praesent congue erat at massa. Sed cursus turpis vitae tortor. Donec posuere vulputate arcu. Phasellus accumsan cursus velit. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Sed aliquam, nisi quis porttitor congue, elit erat euismod orci, ac placerat dolor lectus quis orci. Phasellus consectetuer vestibulum elit. Aenean tellus metus, bibendum sed, posuere ac, mattis non, nunc. Vestibulum fringilla pede sit amet augue. In turpis. Pellentesque posuere. Praesent turpis. Aenean posuere, tor";

char *envp[] = {
    "REQUEST_METHOD=POST",
    "QUERY_STRING=5",
    "CONTENT_LENGTH=3000",
    "CONTENT_TYPE=",
    "SCRIPT_NAME=/cgi-bin/t.py",
    "PATH_INFO=",
    "SERVER_NAME=localhost",
    "SERVER_PORT=8080",
    "SERVER_PROTOCOL=HTTP/1.0",
    "GATEWAY_INTERFACE=CGI/1.1",
    NULL
};
std::string interpreter_g = "/usr/bin/python3";
int cgiHandler(std::string full_path, char **env, std::string interpreter, std::string post_body)
{
	int stdin_pipe[2];
	int stdout_pipe[2];

	pipe(stdin_pipe);
	pipe(stdout_pipe);

	pid_t pid = fork();

	if (pid == 0)
	{
		dup2(stdin_pipe[0], STDIN_FILENO);
		dup2(stdout_pipe[1], STDOUT_FILENO);

		close(stdin_pipe[1]);
		close(stdout_pipe[0]);

		char *args[] = {
			(char*)interpreter.c_str(),
			(char*)full_path.c_str(),
			NULL
		};

		execve(interpreter.c_str(), args, env);
		exit(1);
	}
	time_t start_time = time(NULL);
	bool time_out = false;
	close(stdin_pipe[0]);
	close(stdout_pipe[1]);
	fcntl(stdin_pipe[1], F_SETFL, O_NONBLOCK);
	if (!post_body.empty())
	{
		int size = post_body.size();
		char *data = (char *)post_body.c_str();
		int sent = 0;
		while (sent < size)
		{

			int ret = write(stdin_pipe[1], data + sent, size - sent); 
			if (ret == 0)
				continue;
			else if (ret < 0)
				break;
			sent += ret;
		}
		
	}
	close(stdin_pipe[1]);
	std::cout << "we end writing her" << std::endl;
	char buf[4096];
	int n;
	std::string out;
	fcntl(stdout_pipe[0], F_SETFL, O_NONBLOCK);
	int status;
	while (true)
	{
		int check = waitpid(pid, &status, WNOHANG);
		if (check == pid)
			break;
		n = read(stdout_pipe[0], buf, sizeof(buf));
		if (n > 0)
		{
			out.append(buf, n);
		}
		if (time(NULL) - start_time >= TIME_OUT)
		{
			kill(pid, SIGKILL);
			time_out = true;
			break;
		}
		usleep(1000);
	}
	close(stdout_pipe[0]);
	if (time_out)
	{
		std::cerr << "timouted" << std::endl;
		return -1;
	}
	std::cout << "out : "<< out << "[end]" <<std::endl;
	waitpid(pid, &status, 0);
	if(WIFEXITED(status))
	{
		std::cout << "exited normal " << WEXITSTATUS(status)<<std::endl;
	}
	else if (WIFSIGNALED(status))
	{
		std::cout << "exited signal " << WTERMSIG(status)<<std::endl;
	}
	return 0;
}
int main()
{
	std::cout << post.size()<< cgiHandler(path, envp, interpreter_g, post) << std::endl;
}