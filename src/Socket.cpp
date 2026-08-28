#include "Socket.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>

Socket::Socket() : fd_(::socket(AF_INET, SOCK_STREAM, 0)) {
    if (fd_ == -1) {
        throw std::runtime_error("Socket: failed to create socket");
    }
    // Let us re-bind to the same port immediately after the program
    // restarts, instead of waiting for the OS to release it (TIME_WAIT).
    int opt = 1;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

Socket::Socket(int fd) : fd_(fd) {}

Socket::~Socket() {
    close();
}

Socket::Socket(Socket&& other) noexcept : fd_(other.fd_) {
    other.fd_ = -1;
}

Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        close();
        fd_ = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

void Socket::close() {
    if (fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }
}

void Socket::bindAndListen(int port, int backlog) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(static_cast<uint16_t>(port));

    if (::bind(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        throw std::runtime_error("Socket: bind() failed on port " + std::to_string(port));
    }
    if (::listen(fd_, backlog) < 0) {
        throw std::runtime_error("Socket: listen() failed");
    }
}

Socket Socket::accept() const {
    sockaddr_in clientAddr{};
    socklen_t len = sizeof(clientAddr);
    int clientFd = ::accept(fd_, reinterpret_cast<sockaddr*>(&clientAddr), &len);
    if (clientFd < 0) {
        throw std::runtime_error("Socket: accept() failed");
    }
    return Socket(clientFd);
}

void Socket::connectTo(const std::string& host, int port) const {
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* result = nullptr;
    int err = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &result);
    if (err != 0) {
        throw std::runtime_error("Socket: could not resolve host '" + host + "'");
    }

    bool connected = false;
    for (addrinfo* p = result; p != nullptr; p = p->ai_next) {
        if (::connect(fd_, p->ai_addr, p->ai_addrlen) == 0) {
            connected = true;
            break;
        }
    }
    freeaddrinfo(result);

    if (!connected) {
        throw std::runtime_error("Socket: connect() failed for " + host + ":" + std::to_string(port));
    }
}

void Socket::sendLine(const std::string& text) const {
    std::string withNewline = text + "\n";
    size_t totalSent = 0;
    while (totalSent < withNewline.size()) {
        ssize_t sent = ::send(fd_, withNewline.data() + totalSent,
                               withNewline.size() - totalSent, 0);
        if (sent <= 0) {
            throw std::runtime_error("Socket: send() failed (peer likely disconnected)");
        }
        totalSent += static_cast<size_t>(sent);
    }
}

std::string Socket::receiveLine() const {
    std::string line;
    char c;
    while (true) {
        ssize_t n = ::recv(fd_, &c, 1, 0);
        if (n <= 0) {
            // 0  -> peer closed the connection gracefully
            // <0 -> socket error
            return "";
        }
        if (c == '\n') {
            break;
        }
        line.push_back(c);
    }
    return line;
}

void Socket::shutdown() const {
    if (fd_ != -1) {
        ::shutdown(fd_, SHUT_RDWR);
    }
}
