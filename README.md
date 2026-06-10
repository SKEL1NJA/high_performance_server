# High-Performance C++ HTTP Web Server

A robust, multithreaded HTTP web server built entirely from scratch using C++11 and the Windows Sockets API (Winsock2). This project demonstrates low-level network programming, concurrent client handling, and raw HTTP protocol parsing without relying on heavy web frameworks.

---

## Features

- **TCP Socket Communication:** Initializes and binds native Winsock2 sockets to listen for incoming network traffic.

- **HTTP/1.1 Request Parsing:** Dynamically reads raw byte streams and extracts the HTTP Method, URI, and Version.

- **Static File Serving:** Routes incoming requests to the appropriate `.html` files on disk (e.g., serving `index.html` for root requests) and generates standard `404 Not Found` responses for missing files.

- **Concurrent Multithreading:** Spawns independent, detached threads for every incoming client connection, allowing the server to handle multiple browser requests simultaneously without blocking.

- **Graceful Shutdown:** Implements OS signal handling (`SIGINT`) to safely catch `Ctrl+C` interrupts, forcefully unblock the accept loop, and cleanly release networking resources (`WSACleanup()`) before exiting.

---

## Tech Stack

| Component   | Detail                                  |
|-------------|-----------------------------------------|
| Language    | C++ (C++11 or higher)                   |
| Networking  | Winsock2 API (`<winsock2.h>`, `<ws2tcpip.h>`) |
| Concurrency | C++ Standard Library (`<thread>`, `<atomic>`) |
| Environment | Windows 11, VS Code                     |
| Compiler    | GCC / MinGW                             |

---

## Prerequisites

To build and run this server, you must have:

- A **Windows** operating system.
- **GCC / MinGW** installed and added to your system's `PATH`.

---

## Project Structure

```
high_perf_server/
│
├── main.cpp          # Core server logic (Sockets, Multithreading, Routing)
├── index.html        # Default landing page
├── test-page.html    # Secondary routing test page
├── .gitignore        # Ignores compiled binaries (*.exe, *.o)
└── README.md         # Project documentation
```

---

## How to Build and Run

### 1. Clone the repository

```bash
git clone https://github.com/SKEL1NJA/high_performance_server.git
cd high_perf_server
```

### 2. Compile the server

Open your terminal and use `g++`. You must explicitly link the Winsock library (`-lws2_32`).

```bash
g++ main.cpp -o server.exe -lws2_32
```

### 3. Start the server

```bash
.\server.exe
```

You should see: `Server successfully listening on port 8080.`

### 4. Test it

Open any web browser and navigate to:

| URL | Result |
|-----|--------|
| `http://localhost:8080/` | Loads the Index page |
| `http://localhost:8080/test-page` | Loads the Test page |
| `http://localhost:8080/anything-else` | Returns a 404 Error |

### 5. Stop the server

Press `Ctrl + C` in your terminal. The server will print its cleanup sequence and exit cleanly.