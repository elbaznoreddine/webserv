#include "../headers/Server.hpp"
#include <cstdlib>

int main(int argc, char* argv[]) {
    int port = 8080;

    if (argc > 1) {
        port = std::atoi(argv[1]);
    }

    SimpleServer server(port);

    if (!server.initialize()) {
        return 1;
    }

    server.run();

    return 0;
}