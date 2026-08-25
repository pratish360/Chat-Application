# Terminal Chat Application (C++ / OOP)

A minimal two-person chat app built with TCP sockets. One `chat_server`
process relays messages between exactly two `chat_client` processes.

## Build

```
make
```

This produces two executables: `chat_server` and `chat_client`.

## Run

In one terminal:
```
./chat_server 5555
```

In a second terminal:
```
./chat_client Alice 127.0.0.1 5555
```

In a third terminal:
```
./chat_client Bob 127.0.0.1 5555
```

Once both clients have connected, type a message and press Enter to send it.
Type `/quit` to leave the chat.

## Design / OOP structure

| Class        | Responsibility                                                   |
|--------------|--------------------------------------------------------------------|
| `Socket`     | RAII wrapper around a raw POSIX socket fd (encapsulation) — hides `socket()`, `bind()`, `send()`, `recv()`, etc. behind simple methods, and closes the fd automatically. |
| `Message`    | Bundles sender + content + timestamp, and knows how to format itself into a wire-friendly line and parse itself back. |
| `ChatServer` | Accepts two `Socket`s and relays lines between them, one background thread per direction. |
| `ChatClient` | Connects a `Socket`, then runs a send loop (reads stdin, main thread) and a receive loop (prints incoming lines, background thread) concurrently. |

Each class owns one responsibility and hides its own implementation
details from the rest of the program — that's the encapsulation /
single-responsibility angle you'll want to call out in your report.

## Where to go next (future features)

- File sharing: add a `FileTransfer` class and a message "type" field
  (text vs file) in `Message`.
- More than 2 clients: turn `ChatServer` into a broadcaster with a
  `std::vector<Socket>` of clients instead of a fixed pair — this is
  a natural place to introduce inheritance/polymorphism, e.g. an
  abstract `Room` base class with `BroadcastRoom` / `PrivateRoom`
  subclasses.
- GUI: keep `ChatClient`'s networking logic as-is and swap the
  terminal I/O (`sendLoop`/`receiveLoop`) for a Qt or Dear ImGui
  front end — this is exactly why networking and I/O were kept in
  separate methods.
