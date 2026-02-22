#include "../headers/Server.hpp"

Server::Server(ConfigParser& servers) : _servers(servers)
{}

void Server::startServers()
{
    std::vector<ServerConfig> servers = _servers.getServers();

    int epollfd = epoll_create(1);
    if (epollfd == -1)
        throw std::runtime_error("epoll_create1 failed");
    struct epoll_event ev, ep_events[MAX_EVENTS];

    for (size_t i = 0; i < servers.size(); i++)
    {
        std::vector<Listen> listens = servers[i].getListen();
        for (size_t j = 0; j < listens.size(); j++)
        {
            int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (serverSocket < 0)
                throw std::runtime_error("socket failed to create");

            int flags = fcntl(serverSocket, F_GETFL, 0);
            if (flags < 0 || fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK) < 0)
            {
                close(serverSocket);
                throw std::runtime_error("fcntl failed");
            }

            if (listens[j].host == "localhost")
                listens[j].host = "127.0.0.1";

            sockaddr_in addr = {};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(listens[j].port);
            addr.sin_addr.s_addr = inet_addr(listens[j].host.c_str());

            if (bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
            {
                close(serverSocket);
                throw std::runtime_error("bind failed");
            }
            if (listen(serverSocket, SOMAXCONN) < 0)
            {
                close(serverSocket);
                throw std::runtime_error("listen failed");
            }

            ev.events = EPOLLIN;
            ev.data.fd = serverSocket;
            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, serverSocket, &ev) == -1)
            {
                close(serverSocket);
                throw std::runtime_error("epoll_ctl failed");
            }

            fds.push_back(serverSocket);
			server_fds.push_back(serverSocket);
			std::cout << "Server initialized on " << listens[j].host << ":" << listens[j].port 
                      << " (FD: " << serverSocket << ")\033[0m" << std::endl;
        }
    }

    while (1)
    {
        int nfds = epoll_wait(epollfd, ep_events, MAX_EVENTS, -1);
        if (nfds == -1)
        throw std::runtime_error("epoll_wait failed");

    for (int n = 0; n < nfds; n++)
    {
        int fd = ep_events[n].data.fd;
        if (std::find(server_fds.begin(), server_fds.end(), fd) != server_fds.end())
        {
            int client_fd = accept(fd, NULL, NULL);
            if (client_fd < 0)
                continue;
            int flags = fcntl(client_fd, F_GETFL, 0);
            if (flags < 0 || fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) < 0)
            {
                close(client_fd);
                throw std::runtime_error("fcntl failed");
            }
            ev.events = EPOLLIN;
            ev.data.fd = client_fd;
            epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, &ev);
            fds.push_back(client_fd);
        }
        else
        {
            char buffer[4096] = {'\0'};
            int bytes = recv(fd, buffer, sizeof(buffer), 0);
            if (bytes <= 0)
            {
                epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
            }
            else
            {
                std::string response ="";
                send(fd, response.c_str(), response.size(), 0);
                epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
            }
        }
    }
    }
}

Server::~Server(){}