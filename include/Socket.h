#pragma once
#include <string>

// Socket
// -----------------------------------------------------------------------
// A small RAII wrapper around a POSIX socket file descriptor.
//
// OOP idea being demonstrated: ENCAPSULATION. Nobody outside this class
// needs to know about socket(), bind(), send(), recv(), etc. They just
// call human-readable methods like connectTo() or receiveLine().
//
// The class also manages the lifetime of the underlying file descriptor:
// it is closed automatically in the destructor, so a Socket can never be
// "leaked" as long as it goes out of scope normally.
// -----------------------------------------------------------------------
class Socket {
public:
    // Creates a brand-new, unconnected TCP socket.
    Socket();

    // Wraps an already-existing file descriptor (used internally by
    // accept(), which produces a new connected socket).
    explicit Socket(int fd);

    ~Socket();

    // A Socket owns a raw OS resource (a file descriptor), so copying it
    // doesn't make sense -- copies are disabled, moves are allowed.
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    // ---- Server-side helpers ----
    void bindAndListen(int port, int backlog = 5);
    Socket accept() const;

    // ---- Client-side helper ----
    void connectTo(const std::string& host, int port) const;

    // ---- Shared I/O ----
    // Sends the given text plus a trailing '\n' so the other side can
    // frame messages line-by-line.
    void sendLine(const std::string& text) const;

    // Blocks until a full line (up to '\n') has been received.
    // Returns an empty string if the peer disconnected.
    std::string receiveLine() const;

    // Shuts down both directions of communication (SHUT_RDWR) without
    // fully closing the file descriptor. Unlike close(), this is
    // POSIX-guaranteed to wake up a receiveLine() call currently
    // blocked in *another thread* on this same socket, causing it to
    // return immediately with an empty string -- which is exactly what
    // we need when the main thread wants to make a background receiver
    // thread stop waiting and exit.
    void shutdown() const;

    void close();
    bool isValid() const { return fd_ != -1; }
    int getFd() const { return fd_; }

private:
    int fd_;
};
