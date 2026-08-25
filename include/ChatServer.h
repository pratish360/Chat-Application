#pragma once
#include "Socket.h"

// ChatServer
// -----------------------------------------------------------------------
// Waits for exactly two clients to connect, then relays every line one
// client sends to the other. The server itself never needs to know what
// the messages *mean* -- it just forwards them, which keeps it simple
// and reusable if you later want 3+ clients or a broadcast model.
// -----------------------------------------------------------------------
class ChatServer {
public:
    explicit ChatServer(int port);

    // Blocks: accepts two clients, then relays traffic between them
    // until either one disconnects.
    void run();

private:
    // Runs in its own thread: continuously reads from `from` and
    // forwards each line to `to`. `label` is only used for the
    // server-side console log.
    static void relay(const Socket& from, const Socket& to, const std::string& label);

    int port_;
    Socket listener_;
};
