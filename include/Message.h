#pragma once
#include <string>

// Message
// -----------------------------------------------------------------------
// Represents a single chat message: who sent it, what it says, and when.
// Knows how to turn itself into a single line of text (for sending over
// the socket / printing to the terminal) and how to rebuild itself from
// such a line.
//
// OOP idea being demonstrated: keeping data (sender/content/timestamp)
// and the behavior that operates on it (formatting/parsing) together in
// one class, instead of scattering "string glue" code across main().
// -----------------------------------------------------------------------
class Message {
public:
    Message() = default;
    Message(std::string sender, std::string content);

    const std::string& getSender() const { return sender_; }
    const std::string& getContent() const { return content_; }
    const std::string& getTimestamp() const { return timestamp_; }

    // "[21:03:11] Alice: hello there"
    std::string format() const;

    // Parses a line previously produced by format(). Used by the server
    // if it ever needs to inspect a message instead of just relaying the
    // raw bytes (kept here so the class is symmetrical / reusable).
    static Message parse(const std::string& line);

private:
    std::string sender_;
    std::string content_;
    std::string timestamp_;

    static std::string currentTime();
};
