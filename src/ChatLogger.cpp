#include "ChatLogger.h"
#include <iostream>

ChatLogger::ChatLogger(const std::string& filename)
    : file_(filename, std::ios::app) {
    if (!file_.is_open()) {
        std::cerr << "[server] warning: could not open log file '" << filename
                   << "', chat history will not be saved.\n";
    }
}

void ChatLogger::log(const std::string& line) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (file_.is_open()) {
        // std::endl flushes immediately, so the file is up to date on
        // disk even if the server is later killed rather than shut
        // down cleanly.
        file_ << line << std::endl;
    }
}
