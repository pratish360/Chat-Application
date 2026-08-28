#pragma once
#include "Socket.h"
#include "ChatLogger.h"
#include <string>

// ChatServer
// -----------------------------------------------------------------------
// Waits for exactly two clients to connect, then relays every line one
// client sends to the other. The server itself never needs to know what
// most messages *mean* -- it just forwards them -- except for two small
// pieces of protocol it does understand:
//
//   1. A one-time handshake line ("NAME:<name>") each client sends right
//      after connecting, so the server knows who is who.
//   2. A "/list" command a client can send instead of a chat message,
//      which the server answers directly (it is not relayed to the
//      other client).
//
// Every ordinary chat line that IS relayed is also appended to a log
// file on disk via ChatLogger, so a transcript of the conversation
// survives after the programs exit.
// -----------------------------------------------------------------------
class ChatServer {
public:
    explicit ChatServer(int port, std::string logFile = "chat_history.log");

    // Blocks: accepts two clients, then relays traffic between them
    // until either one disconnects.
    void run();

private:
    // Runs in its own thread: continuously reads from `from` and
    // forwards each line to `to`, except for recognized commands
    // (currently just "/list"), which are answered directly instead.
    void relay(const Socket& from, const Socket& to, const std::string& fromLabel);

    // Reads the expected "NAME:<name>" handshake line from a
    // newly-connected client. Falls back to `label` if the client
    // didn't send one (e.g. an older/different client program).
    std::string performHandshake(const Socket& client, const std::string& label);

    int port_;
    Socket listener_;
    ChatLogger logger_;
    std::string name1_;
    std::string name2_;
};
