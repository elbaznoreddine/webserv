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
    Request req;
    while (true)
    {
        socklen_t addrlen = sizeof(address);
        int client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

        if (client_socket < 0)
            continue;

        char buffer[4096];
        std::memset(buffer, 0, sizeof(buffer));

        int bytes = read(client_socket, buffer, sizeof(buffer) - 1);
		int i = 0;
		while (buffer[i])
        {
            ss << buffer[i];
			// vec.push_back(buffer[i]);
            i++;
        }

        

        // if (bytes > 0)
        // {
            // buffer[bytes] = '\0';

            // // 🔥 RAW REQUEST (NO parsing)
            // std::cout << "===== RAW REQUEST =====" << std::endl;
            // std::cout << buffer << std::endl;
            // // 🔥 MANUAL HTTP/1.0 RESPONSE
            // std::string response =
            //     "HTTP/1.0 200 OK\r\n"
            //     "Content-Type: text/plain\r\n"
            //     "Content-Length: 13\r\n"
            //     "\r\n"
            //     "Hello, world!";

            // send(client_socket, response.c_str(), response.size(), 0);
			// Request req;
			// Response res;

			// std::ifstream file("req");
			// req.parser(buffer);
        // }

		// if (bytes > 0)
		// {
			// for (std::vector<char>::iterator it = vec.begin(); it != vec.end(); it++)
            // {
            //     if (*it == '\r')
            //         std::cout << "\\r";
            //     else
			// 	    std::cout << *it;
            // }
            // std::cout << std::endl;
			// close(client_socket); // HTTP/1.0 closes connection
		// }
        (void)bytes;
        std::string res = req.parser(ss);
        send(client_socket, res.c_str(), res.size(), 0);
        close(client_socket);
    }
    close(server_fd);
    return 0;
}
