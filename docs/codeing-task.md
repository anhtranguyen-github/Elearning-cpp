### Coding Tasks for AI

1. Implement the **server**:
    - Use **I/O multiplexing** (`select`/`poll`/`epoll`) to manage all active sockets.
    - Handle accept, read, write, and disconnect events.
    - Parse messages according to the defined protocol.
    - Maintain user state in memory.
    - Log all network activity to `logs/server.log`.
2. Implement the **client**:
    - Provide a command-line or Qt-based UI.
    - Connect to the server using POSIX sockets.
    - Send and receive protocol messages.
    - Log all activity to `logs/client.log`.
3. Implement **Protocol Layer**:
    - Define message structs and parsing logic.
    - Serialize/deserialize messages.
    - Handle message delimiting and framing.
4. Implement **Utility and Error Handling**:
    - Centralized logger (`Logger` class).
    - Input validation, socket cleanup, graceful shutdown.
5. Generate a **CMakeLists.txt** to build both server and client targets.
6. Include concise inline comments explaining each class and function.

---

### Additional Constraints

- Use only **standard C++ + POSIX APIs + Qt Widgets** (no Boost, no Asio, no external network libraries).
- Must compile and run on both **Windows** and **Linux**.
- Keep code modular and maintainable (OOP recommended).
- Avoid thread-per-client model — main server loop must rely on multiplexing.
- Prefer stable and simple logic over over-optimization.

---

### Output Requirements

When generating code:

- Output full folder-by-folder implementation.
- Each file should contain working C++ code with comments.
- Root CMakeLists.txt should correctly build both executables:
    - `server`
    - `client`
- Explain dependencies or platform-specific macros where needed (`_WIN32`, `__linux__`).