#include "ChatClient.h"
#include "Message.h"

#include <iostream>
#include <thread>

ChatClient::ChatClient(std::string name, std::string host, int port)
    : name_(std::move(name)), host_(std::move(host)), port_(port), running_(true) {}

void ChatClient::receiveLoop() {
    while (running_) {
        std::string line = socket_.receiveLine();
        if (line.empty()) {
            if (running_) {
                std::cout << "\n[system] Peer disconnected. Press Enter to exit.\n";
            }
            running_ = false;
            break;
        }
        // Move to a fresh line so the incoming message doesn't collide
        // with whatever the user is currently typing.
        std::cout << "\r" << line << "\n" << name_ << "> " << std::flush;
    }
}

void ChatClient::sendLoop() {
    std::string input;
    while (running_) {
        std::cout << name_ << "> " << std::flush;
        if (!std::getline(std::cin, input)) {
            break; // stdin closed (e.g. Ctrl+D)
        }
        if (input == "/quit") {
            break;
        }
        if (input.empty()) {
            continue;
        }

        Message msg(name_, input);
        try {
            socket_.sendLine(msg.format());
        } catch (const std::exception&) {
            std::cout << "[system] Failed to send, connection lost.\n";
            break;
        }
    }
    running_ = false;
}

void ChatClient::run() {
    std::cout << "[client] connecting to " << host_ << ":" << port_ << " ...\n";
    socket_.connectTo(host_, port_);
    std::cout << "[client] connected! Type a message and press Enter. Type /quit to leave.\n";

    std::thread receiver(&ChatClient::receiveLoop, this);
    sendLoop(); // runs on the main thread, reading from stdin

    running_ = false;
    socket_.close(); // unblocks receiveLoop's pending recv()
    if (receiver.joinable()) {
        receiver.join();
    }
    std::cout << "[client] disconnected. Goodbye, " << name_ << "!\n";
}
