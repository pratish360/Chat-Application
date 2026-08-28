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
(Optionally pass a second argument for the log file name, e.g. `./chat_server 5555 my_log.log` — it defaults to `chat_history.log`.)

In a second terminal:
```
./chat_client Alice 127.0.0.1 5555
```

In a third terminal:
```
./chat_client Bob 127.0.0.1 5555
```

Once both clients have connected, type a message and press Enter to send it.

**Commands:**
- `/list` — ask the server who's currently connected
- `/quit` — leave the chat

**Chat history:** every relayed message is appended to `chat_history.log` (in the folder you ran `chat_server` from), so a transcript survives after both clients disconnect.

## Design / OOP structure

| Class        | Responsibility                                                   |
|--------------|--------------------------------------------------------------------|
| `Socket`     | RAII wrapper around a raw POSIX socket fd (encapsulation) — hides `socket()`, `bind()`, `send()`, `recv()`, etc. behind simple methods, and closes the fd automatically. |
| `Message`    | Bundles sender + content + timestamp, and knows how to format itself into a wire-friendly line and parse itself back. |
| `ChatLogger` | Thread-safe append-only file writer; owns the log file handle so `ChatServer` doesn't need to think about locking. |
| `ChatServer` | Accepts two `Socket`s, performs a one-time name handshake with each, relays lines between them (one background thread per direction), answers `/list` directly, and logs every relayed chat line via `ChatLogger`. |
| `ChatClient` | Connects a `Socket`, sends a `NAME:` handshake, then runs a send loop (reads stdin, main thread) and a receive loop (prints incoming lines, background thread) concurrently. |

### Protocol notes

- On connect, each client immediately sends `NAME:<name>` so the server can identify it (used by `/list`).
- A client typing `/list` sends the literal text `/list` instead of a formatted chat message; the server intercepts it, replies directly to that client only, and never relays or logs it.
- Everything else typed by the user is wrapped by `Message` into `[HH:MM:SS] name: content` before being sent, relayed as-is, and logged as-is.

Each class owns one responsibility and hides its own implementation
details from the rest of the program — that's the encapsulation /
single-responsibility angle you'll want to call out in your report.
