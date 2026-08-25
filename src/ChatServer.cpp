#include "ChatServer.h"

#include <iostream>
#include <thread>

ChatServer::ChatServer(int port) : port_(port) {}

void ChatServer::relay(const Socket& from, const Socket& to, const std::string& label) {
    while (true) {
        std::string line = from.receiveLine();
        if (line.empty()) {
            // Empty line back from receiveLine() means the peer hung up.
            std::cout << "[server] " << label << " disconnected.\n";
            break;
        }
        std::cout << "[server] relaying " << label << " -> " << line << "\n";
        try {
            to.sendLine(line);
        } catch (const std::exception&) {
            std::cout << "[server] could not deliver message, other client is gone.\n";
            break;
        }
    }
}

void ChatServer::run() {
    listener_.bindAndListen(port_);
    std::cout << "[server] listening on port " << port_ << " ...\n";

    std::cout << "[server] waiting for client #1...\n";
    Socket client1 = listener_.accept();
    std::cout << "[server] client #1 connected.\n";

    std::cout << "[server] waiting for client #2...\n";
    Socket client2 = listener_.accept();
    std::cout << "[server] client #2 connected. Chat can begin.\n";

    // Relay in both directions concurrently.
    std::thread t1(relay, std::cref(client1), std::cref(client2), "client#1");
    std::thread t2(relay, std::cref(client2), std::cref(client1), "client#2");

    t1.join();
    t2.join();

    std::cout << "[server] session ended, shutting down.\n";
}
