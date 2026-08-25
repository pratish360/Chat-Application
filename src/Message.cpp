#include "Message.h"

#include <ctime>
#include <sstream>
#include <iomanip>

Message::Message(std::string sender, std::string content)
    : sender_(std::move(sender)),
      content_(std::move(content)),
      timestamp_(currentTime()) {}

std::string Message::currentTime() {
    std::time_t t = std::time(nullptr);
    std::tm tmBuf{};
    localtime_r(&t, &tmBuf);
    std::ostringstream oss;
    oss << std::put_time(&tmBuf, "%H:%M:%S");
    return oss.str();
}

std::string Message::format() const {
    // Example: "[21:03:11] Alice: hello there"
    return "[" + timestamp_ + "] " + sender_ + ": " + content_;
}

Message Message::parse(const std::string& line) {
    Message m;
    // Expected shape: "[HH:MM:SS] sender: content"
    size_t closeBracket = line.find(']');
    if (line.empty() || line.front() != '[' || closeBracket == std::string::npos) {
        // Not in the expected format -- treat the whole line as content
        // from an "unknown" sender rather than crashing.
        m.sender_ = "unknown";
        m.content_ = line;
        m.timestamp_ = currentTime();
        return m;
    }

    m.timestamp_ = line.substr(1, closeBracket - 1);

    size_t colon = line.find(':', closeBracket);
    if (colon == std::string::npos) {
        m.sender_ = "unknown";
        m.content_ = line.substr(closeBracket + 1);
        return m;
    }

    // "] Alice: hello" -> sender starts 2 chars after ']' (skip "] ")
    m.sender_ = line.substr(closeBracket + 2, colon - (closeBracket + 2));
    m.content_ = line.substr(colon + 2); // skip ": "
    return m;
}
