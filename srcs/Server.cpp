#include "../headers/Server.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

SimpleServer::SimpleServer(int p) : server_fd(-1), port(p) {}

SimpleServer::~SimpleServer() {
    cleanup();
}

bool SimpleServer::initialize() {
    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Error: socket creation failed" << std::endl;
        return false;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Error: setsockopt failed" << std::endl;
        return false;
    }

    // Make socket non-blocking
    if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "Error: fcntl failed" << std::endl;
        return false;
    }

    // Bind socket
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Error: bind failed on port " << port << std::endl;
        return false;
    }

    // Listen
    if (listen(server_fd, 10) < 0) {
        std::cerr << "Error: listen failed" << std::endl;
        return false;
    }

    // Add server socket to poll
    pollfd pfd;
    pfd.fd = server_fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    poll_fds.push_back(pfd);

    std::cout << "Server initialized on port " << port << std::endl;
    return true;
}

void SimpleServer::run() {
    std::cout << "Server running... Press Ctrl+C to stop" << std::endl;

    while (true) {
        // Wait for events (-1 = infinite timeout)
        int ret = poll(poll_fds.data(), poll_fds.size(), -1);

        if (ret < 0) {
            std::cerr << "Error: poll failed" << std::endl;
            break;
        }

        // Check all file descriptors
        for (size_t i = 0; i < poll_fds.size(); i++) {
            if (poll_fds[i].revents == 0)
                continue;

            // Server socket - new connection
            if (poll_fds[i].fd == server_fd) {
                acceptNewConnection();
            }
            // Client socket - data to read or write
            else {
                if (poll_fds[i].revents & POLLIN) {
                    handleClientRead(poll_fds[i].fd, i);
                }
                if (poll_fds[i].revents & POLLOUT) {
                    handleClientWrite(poll_fds[i].fd, i);
                }
                // Handle errors/hangup
                if (poll_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                    closeClient(poll_fds[i].fd, i);
                }
            }
        }
    }
}

void SimpleServer::handleClientRead(int client_fd, size_t index) {
    char buffer[4096];
    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes <= 0) {
        if (bytes == 0) {
            std::cout << "Client " << client_fd << " disconnected" << std::endl;
        } else {
            std::cerr << "Error: recv failed for client " << client_fd << std::endl;
        }
        closeClient(client_fd, index);
        return;
    }

    buffer[bytes] = '\0';

    // Append to client buffer
    client_buffers[client_fd] += std::string(buffer, bytes);

    std::cout << "Received " << bytes << " bytes from client " << client_fd << std::endl;

    //should implement proper HTTP request parsing here
    // For now, just check if we have a complete request (ends with \r\n\r\n)
    if (client_buffers[client_fd].find("\r\n\r\n") != std::string::npos) {
        std::cout << "Complete request received:\n" << client_buffers[client_fd] << std::endl;

        // Enable POLLOUT to send response
        poll_fds[index].events |= POLLOUT;
    }
}

void SimpleServer::handleClientWrite(int client_fd, size_t index) {
    // should build proper HTTP responses here
    // This is just a simple example response

    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 51\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<html><body><h1>Hello World!</h1></body></html>";

    ssize_t sent = send(client_fd, response.c_str(), response.length(), 0);

    if (sent < 0) {
        std::cerr << "Error: send failed" << std::endl;
    } else {
        std::cout << "Sent " << sent << " bytes to client " << client_fd << std::endl;
    }

    // Close connection after sending (HTTP/1.0)
    closeClient(client_fd, index);
}

void SimpleServer::acceptNewConnection() {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);

    if (client_fd < 0) {
        std::cerr << "Error: accept failed" << std::endl;
        return;
    }
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "Error: fcntl failed for client" << std::endl;
        close(client_fd);
        return;
    }

    // Add to poll
    pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;//only interested in reading
    pfd.revents = 0;
    poll_fds.push_back(pfd);

    client_buffers[client_fd] = "";

    std::cout << "New client connected: " << client_fd << std::endl;
}

void SimpleServer::closeClient(int client_fd, size_t index) {
    std::cout << "Closing client " << client_fd << std::endl;
    close(client_fd);
    client_buffers.erase(client_fd);
    poll_fds.erase(poll_fds.begin() + index);
}

void SimpleServer::cleanup()
{
    for (size_t i = 1; i < poll_fds.size(); i++)
{
        close(poll_fds[i].fd);
    }
    if (server_fd >= 0)
{
        close(server_fd);
    }
}