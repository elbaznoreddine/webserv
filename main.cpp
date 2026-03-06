#include "Request.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

struct Config
{
    int port;
};

Config parseConfig(const std::string &filename)
{
    Config config;
    config.port = 8080; // default

    std::ifstream file(filename.c_str());
    std::string line;

    while (std::getline(file, line))
    {
        std::size_t pos = line.find("listen");
        if (pos != std::string::npos)
        {
            std::string number = line.substr(pos + 6);
            config.port = std::atoi(number.c_str());
        }
    }
    return config;
}

int main()
{
    Config config = parseConfig("server.conf");

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(config.port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed\n";
        return 1;
    }

    if (listen(server_fd, 10) < 0)
    {
        std::cerr << "Listen failed\n";
        return 1;
    }

    std::cout << "Listening on port " << config.port << std::endl;

    std::stringstream ss;
    std::stringstream out0;
    std::string str = "";
    Request req;
    while (true)
    {
        socklen_t addrlen = sizeof(address);
        int client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

        if (client_socket < 0)
            continue;

        char buffer[1024];
        std::memset(buffer, 0, sizeof(buffer));
        int bytes = read(client_socket, buffer, sizeof(buffer) - 1);
        std::string s;
		while (true)
        {
            str += buffer;
            size_t len = str.find("\r\n\r\n");
            if (len != std::string::npos)
            {
                ss << str.substr(0, len);
                s = buffer;
                for (int i = s.find("\r\n\r\n") + 4; i < bytes; i++)
                    out0 << buffer[i];
                break ;
            }
            if (bytes != 1023)
                break ;
            bytes = read(client_socket, buffer, sizeof(buffer) - 1);
        }
        req.parser(ss);
        if (req.getMethod() == "POST" && req.getStatus() == "200")
        {
            std::ofstream out("out", std::ios::binary);
            std::string file((std::istreambuf_iterator<char>(out0)),std::istreambuf_iterator<char>());
            out << file;
            if (bytes == 1023)
            {
                bytes = read(client_socket, buffer, sizeof(buffer) - 1);
                while (true)
                {
                    for (int i = 0; i < bytes; i++)
                        out << buffer[i];
                    if (bytes != 1023)
                        break ;
                    bytes = read(client_socket, buffer, sizeof(buffer) - 1);
                }
            }
        }
        std::string str1 = req.res.getRes();
        while (true)
        {
            if (str1.size() == 0)
                break ;
            if (str1.size() > 1000000)
            {
                str = str1.substr(0, 1000000);
                str1.erase(0, 1000000);
            }
            else
            {
                str = str1;
                str1.erase(0, str1.size());
            }
            send(client_socket, str.c_str(), str.size(), 0);
        }
        close(client_socket);
        exit(0);
    }
    close(server_fd);
    return 0;
}
