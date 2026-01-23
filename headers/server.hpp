#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>

class SimpleServer {
private:
	int server_fd;
	int port;
	std::vector<pollfd> poll_fds;
	std::map<int, std::string> client_buffers; // Store partial requests

	// Private helper methods
	void acceptNewConnection();
	void closeClient(int client_fd, size_t index);
	void cleanup();

public:
	// Constructor & Destructor
	SimpleServer(int p = 8080);
	~SimpleServer();

	// Initialization and main loop
	bool initialize();
	void run();

	// Client handling methods (your friend will expand these)
	void handleClientRead(int client_fd, size_t index);
	void handleClientWrite(int client_fd, size_t index);
};

#endif // SIMPLESERVER_HPP