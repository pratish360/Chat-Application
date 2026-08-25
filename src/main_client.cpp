#include "ChatClient.h"

#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    // Usage: chat_client <name> [host] [port]
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <your_name> [host=127.0.0.1] [port=5555]\n";
        return 1;
    }

    std::string name = argv[1];
    std::string host = (argc >= 3) ? argv[2] : "127.0.0.1";
    int port = (argc >= 4) ? std::atoi(argv[3]) : 5555;

    try {
        ChatClient client(name, host, port);
        client.run();
    } catch (const std::exception& e) {
        std::cerr << "[client] fatal error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
