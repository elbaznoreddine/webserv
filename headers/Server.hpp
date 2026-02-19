#pragma once 

#include "ConfigParser.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <sys/epoll.h>
#include <fcntl.h>
#include <algorithm>

class Server
{
	private:
		std::vector<int> fds;
		// std::vector<int> epoll_fds;
		// std::vector<int> epoll_client_fds;
		// std::vector<int> epoll_server_fds;
		std::vector<int> server_fds; 
		ConfigParser& _servers;
	public:
		Server(ConfigParser& servers);
		void startServers();
		~Server();
};