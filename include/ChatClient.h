#pragma once
#include "Socket.h"
#include <atomic>
#include <string>

// ChatClient
// -----------------------------------------------------------------------
// Connects to a ChatServer, then runs two things concurrently:
//   1. sendLoop()    (main thread) reads lines the user types and sends them
//   2. receiveLoop() (background thread) prints whatever arrives from the peer
//
// Splitting these into two threads is what lets you type a message and
// receive one at the same time, instead of blocking on one or the other.
// -----------------------------------------------------------------------
class ChatClient {
public:
    ChatClient(std::string name, std::string host, int port);

    // Connects and runs the chat session until the user types /quit or
    // the peer disconnects.
    void run();

private:
    void receiveLoop();
    void sendLoop();

    std::string name_;
    std::string host_;
    int port_;
    Socket socket_;
    std::atomic<bool> running_;
};
