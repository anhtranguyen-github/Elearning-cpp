You are an experienced C++ network systems engineer.

Implement a complete **client–server application** using **C++17**, **POSIX sockets**, and a **custom application protocol**.

Follow all requirements strictly — do not change structure, naming conventions, or architecture.

---

### Project Context

- **Language:** C++17
- **Target OS:** Windows and Linux (cross-platform)
- **Build system:** CMake
- **Networking:** POSIX socket API (`socket`, `bind`, `listen`, `accept`, `send`, `recv`)
- **Concurrency model:** **I/O Multiplexing**
    - Use `select()`, `poll()`, or `epoll()` to handle multiple client sockets concurrently.
    - The server must run efficiently with a single main thread managing all clients.
    - No thread-per-client model. Only minimal threads for background tasks (optional logging or admin console).
- **UI Framework:** Qt Widgets (`.ui` XML layout) for the client interface.
- **Purpose:** Build a robust application with a self-defined **application-level protocol** (e.g., login, messaging, game commands, etc.).

