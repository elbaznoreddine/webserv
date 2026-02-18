#include "../headers/Server.hpp"

Server::Server(ConfigParser& servers) : _servers(servers)
{}

void Server::startServers()
{
	std::vector<ServerConfig> servers = _servers.getServers();
	for (size_t i = 0; i < servers.size(); i++)
	{
		std::cout << "server :" << i<<std::endl;
		for (size_t j = 0; j < servers[i].getListen().size(); j++)
		{
			std::cout << "	listen :" << j<<std::endl;
			std::vector<Listen> listens = servers[i].getListen();
			int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
			if (serverSocket < 0)
				throw std::runtime_error("socket faild to create");
			fds.push_back(serverSocket);
			if (listens[j].host == "localhost")
        		listens[j].host = "127.0.0.1";
			sockaddr_in serverAddress;
			serverAddress.sin_family = AF_INET;
			serverAddress.sin_port = htons(listens[j].port);
			serverAddress.sin_addr.s_addr = inet_addr(listens[j].host.c_str());
			if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
			{
				std::stringstream os ;
				os << listens[j].port;
				close(serverSocket);
				throw std::runtime_error("Binding of " +listens[j].host + ":" + os.str() + "failed");
			}
			std::cout << "Binding of " << listens[j].host
					<< ":" << listens[j].port << "..."
					<<std::endl;

			if (listen(serverSocket, SOMAXCONN) < 0)
			{
				std::stringstream os ;
				os << listens[j].port;
				close(serverSocket);
				throw std::runtime_error("Listening of " +listens[j].host + ":" + os.str() + "failed");
			}
			std::cout << "Listening of " << listens[j].host
					<< ":" << listens[j].port << "..."
					<<std::endl;
			
		}
		
	}
}

Server::~Server(){}