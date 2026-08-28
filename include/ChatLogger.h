#pragma once
#include <fstream>
#include <mutex>
#include <string>

// ChatLogger
// -----------------------------------------------------------------------
// A small, thread-safe writer that appends chat lines to a log file on
// disk. The server has two relay threads running at once (one per
// direction of conversation), and both may want to log a line at
// roughly the same time -- without a mutex, their writes could
// interleave mid-line. This class exists purely to own that file handle
// and guard it with a lock, so the rest of ChatServer doesn't need to
// think about thread-safety at all.
//
// OOP idea being demonstrated: single responsibility (this class does
// exactly one thing) plus RAII (the ofstream closes itself when a
// ChatLogger is destroyed -- no manual cleanup needed).
// -----------------------------------------------------------------------
class ChatLogger {
public:
    // Opens (or creates) the given file in append mode, so restarting
    // the server doesn't erase previous history.
    explicit ChatLogger(const std::string& filename);

    // Appends one line (plus a newline) to the log file. Safe to call
    // from multiple threads concurrently.
    void log(const std::string& line);

private:
    std::ofstream file_;
    std::mutex mutex_;
};
