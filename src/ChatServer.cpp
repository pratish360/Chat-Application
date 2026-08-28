#include "ChatServer.h"

#include <iostream>
#include <thread>

namespace {
constexpr const char* kNamePrefix = "NAME:";
constexpr const char* kListCommand = "/list";
}  // namespace

ChatServer::ChatServer(int port, std::string logFile)
    : port_(port), logger_(logFile) {}

std::string ChatServer::performHandshake(const Socket& client, const std::string& label) {
    std::string line = client.receiveLine();
    // A well-behaved client's very first line is "NAME:<their name>".
    if (line.rfind(kNamePrefix, 0) == 0) {  // rfind(..., 0) == "starts with"
        return line.substr(std::string(kNamePrefix).size());
    }
    std::cout << "[server] " << label << " did not send a name handshake, using default name.\n";
    return label;
}

void ChatServer::relay(const Socket& from, const Socket& to, const std::string& fromLabel) {
    while (true) {
        std::string line = from.receiveLine();
        if (line.empty()) {
            std::cout << "[server] " << fromLabel << " disconnected.\n";
            break;
        }

        if (line == kListCommand) {
            // Answered directly to the asking client -- never relayed
            // to the other client, and not logged as a chat message.
            std::string reply = "[server] Connected users: " + name1_ + ", " + name2_;
            try {
                from.sendLine(reply);
            } catch (const std::exception&) {
                break;
            }
            continue;
        }

        std::cout << "[server] relaying " << fromLabel << " -> " << line << "\n";
        logger_.log(line);
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
    name1_ = performHandshake(client1, "client#1");
    std::cout << "[server] client #1 connected as '" << name1_ << "'.\n";

    std::cout << "[server] waiting for client #2...\n";
    Socket client2 = listener_.accept();
    name2_ = performHandshake(client2, "client#2");
    std::cout << "[server] client #2 connected as '" << name2_ << "'. Chat can begin.\n";

    // Relay in both directions concurrently.
    std::thread t1(&ChatServer::relay, this, std::cref(client1), std::cref(client2), name1_);
    std::thread t2(&ChatServer::relay, this, std::cref(client2), std::cref(client1), name2_);

    t1.join();
    t2.join();

    std::cout << "[server] session ended, shutting down.\n";
}
