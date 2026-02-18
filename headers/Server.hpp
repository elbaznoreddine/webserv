#pragma once 

#include "ConfigParser.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <fcntl.h>

class Server
{
	private:
		std::vector<int> fds;
		ConfigParser& _servers;
	public:
		Server(ConfigParser& servers);
		void startServers();
		~Server();
};