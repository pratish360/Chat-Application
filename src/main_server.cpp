#include "ChatServer.h"

#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    int port = 5555; // default port
    std::string logFile = "chat_history.log"; // default log file

    if (argc >= 2) {
        port = std::atoi(argv[1]);
    }
    if (argc >= 3) {
        logFile = argv[2];
    }

    try {
        ChatServer server(port, logFile);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "[server] fatal error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
