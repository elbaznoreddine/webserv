#include "../headers/Server.hpp"

Server::Server(ConfigParser& servers) : _servers(servers)
{}

void Server::startServers()
{
    signal(SIGQUIT, SIG_IGN);
    std::vector<ServerConfig> servers = _servers.getServers();
    int epollfd = epoll_create(1);
    if (epollfd == -1)
        throw std::runtime_error("epoll_create failed");

    struct epoll_event ev;
    struct epoll_event ep_events[MAX_EVENTS];

    for (size_t i = 0; i < servers.size(); i++)
    {
        std::vector<Listen> listens = servers[i].getListen();
        for (size_t j = 0; j < listens.size(); j++)
        {
            int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (serverSocket < 0)
            {
                close(epollfd);
                throw std::runtime_error("socket failed to create");
            }
            int opt = 1;
            if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
            {
                close(serverSocket);
                close(epollfd);
                throw std::runtime_error("setsockopt failed");
            }
            int flags = fcntl(serverSocket, F_GETFL, 0);
            if (flags < 0 || fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK) < 0)
            {
                close(serverSocket);
                close(epollfd);
                throw std::runtime_error("fcntl failed");
            }
            if (listens[j].host == "localhost")
                listens[j].host = "127.0.0.1";
            sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(listens[j].port);
            addr.sin_addr.s_addr = inet_addr(listens[j].host.c_str());

            if (addr.sin_addr.s_addr == INADDR_NONE)
            {
                close(serverSocket);
                close(epollfd);
                throw std::runtime_error("invalid host: " + listens[j].host);
            }
            if (bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
            {
                close(serverSocket);
                close(epollfd);
                throw std::runtime_error("bind failed on " + listens[j].host + ":" + 
                                        static_cast<std::ostringstream&>(
                                        std::ostringstream() << listens[j].port).str());
            }
            if (listen(serverSocket, SOMAXCONN) < 0)
            {
                close(serverSocket);
                close(epollfd);
                throw std::runtime_error("listen failed");
            }
            ev.events = EPOLLIN;
            ev.data.fd = serverSocket;
            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, serverSocket, &ev) == -1)
            {
                close(serverSocket);
                close(epollfd);
                throw std::runtime_error("epoll_ctl failed");
            }
            fds.push_back(serverSocket);
            server_fds.push_back(serverSocket);

            std::cout << "Server initialized on " 
                    << listens[j].host << ":" << listens[j].port
                    << " (FD: " << serverSocket << ")" << std::endl;
        }
    }

    while (1)
    {
        int nfds = epoll_wait(epollfd, ep_events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            if (errno == EINTR)
                continue;
            throw std::runtime_error("epoll_wait failed");
        }

        for (int n = 0; n < nfds; n++)
        {
            int fd = ep_events[n].data.fd;
            if (ep_events[n].events & (EPOLLERR | EPOLLHUP))
            {
                epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
                continue;
            }

            if (std::find(server_fds.begin(), server_fds.end(), fd) != server_fds.end())
            {
                while (1)
                {
                    sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_len);
                    
                    if (client_fd < 0)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                            break;
                        std::cerr << "accept failed: " << std::endl;
                        break;
                    }
                    int flags = fcntl(client_fd, F_GETFL, 0);
                    if (flags < 0 || fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) < 0)
                    {
                        std::cerr << "fcntl failed on client_fd" << std::endl;
                        close(client_fd);
                        continue;
                    }
                    struct epoll_event client_ev;
                    client_ev.events = EPOLLIN;
                    client_ev.data.fd = client_fd;
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, &client_ev) == -1)
                    {
                        std::cerr << "epoll_ctl failed for client" << std::endl;
                        close(client_fd);
                        continue;
                    }

                    fds.push_back(client_fd);
                    std::cout << "New client connected: fd=" << client_fd
                            << " from " << inet_ntoa(client_addr.sin_addr)
                            << ":" << ntohs(client_addr.sin_port) << std::endl;
                }
            }
            else if (ep_events[n].events & EPOLLIN)
            {
                std::string raw_request;
                char buffer[4096];
                
                while (1)
                {
                    memset(buffer, 0, sizeof(buffer));
                    int bytes = recv(fd, buffer, sizeof(buffer), 0);

                    if (bytes < 0)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                            break;
                        std::cerr << "recv failed: "  << std::endl;
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
                        close(fd);
                        raw_request.clear();
                        break;
                    }
                    else if (bytes == 0)
                    {
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
                        close(fd);
                        raw_request.clear();
                        break;
                    }
                    else
                    {
                        raw_request.append(buffer, bytes);
                        if (bytes < (int)sizeof(buffer))
                            break;
                    }
                }
                if (!raw_request.empty())
                {
                    std::cout << raw_request << raw_request.size()<<std::endl;
                    std::string response = "";
                    size_t total_sent = 0;
                    while (total_sent < response.size())
                    {
                        int sent = send(fd, response.c_str() + total_sent,
                                    response.size() - total_sent, 0);
                        if (sent < 0)
                        {
                            if (errno == EAGAIN || errno == EWOULDBLOCK)
                                continue;
                            std::cerr << "send failed: "  << std::endl;
                            break;
                        }
                        total_sent += sent;
                    }
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                }
            }
        }
    }
}

Server::~Server(){}

