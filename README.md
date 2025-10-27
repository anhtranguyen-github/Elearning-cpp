# English Learning Platform - Client-Server Application

A comprehensive client-server application for English learning, built with C++17, POSIX sockets, and Qt Widgets.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Requirements](#requirements)
- [Building](#building)
- [Running](#running)
- [Project Structure](#project-structure)
- [Protocol Design](#protocol-design)
- [Usage Examples](#usage-examples)
- [Platform Support](#platform-support)
- [Troubleshooting](#troubleshooting)

## Overview

This is a full-featured English learning platform implementing a custom application protocol over TCP sockets. The system uses I/O multiplexing (select/poll) for efficient concurrent client handling without thread-per-client overhead.

**Key Technologies:**
- C++17
- POSIX Sockets API
- I/O Multiplexing (select on Windows, poll on Linux)
- Qt5 Widgets for GUI
- CMake build system
- Custom binary protocol with text framing

## Features

### Student Features
- **Account Management**: Register and login
- **Study Setup**: Select proficiency level (Beginner, Intermediate, Advanced)
- **Content Access**: Browse and view lessons by level
- **Exercises & Tests**: Submit quizzes, exercises, and receive scores
- **Games**: Word matching, sentence matching, picture matching games
- **Communication**: Chat with other users and teachers
- **Assessment**: View scores and receive feedback from teachers

### Teacher Features
- **Feedback**: Provide feedback to student exercises
- **Communication**: Chat with students

### Admin Features
- **Game Management**: Add items to games
- **System Monitoring**: Server-side logging of all activities

## Architecture

### Server Architecture
- **I/O Multiplexing**: Uses `select()` on Windows and `poll()` on Linux
- **Single-threaded**: Main loop handles all clients efficiently
- **Non-blocking sockets**: Prevents any client from blocking others
- **Session management**: In-memory user sessions with timeout handling
- **Protocol handling**: Message framing with delimiter-based parsing

### Client Architecture
- **Qt-based GUI**: Modern tabbed interface for all features
- **Asynchronous communication**: Non-blocking socket operations
- **Heartbeat mechanism**: Keeps connection alive
- **Automatic reconnection**: Handles network failures gracefully

### Protocol Design
- **Message Format**: `TYPE|LENGTH|SEQUENCE|PAYLOAD\n`
- **Type codes**: 16-bit message type identifiers (0x0101-0x0921)
- **Framing**: Newline-delimited messages
- **Stateful**: Server maintains user sessions
- **Error handling**: Comprehensive error codes and responses

## Requirements

### Common Requirements
- CMake 3.10 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)

### Linux
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake

# For Qt GUI client (optional)
sudo apt-get install qtbase5-dev qtbase5-dev-tools
```

### Windows
- Visual Studio 2017 or higher (with C++ tools)
- CMake (via Visual Studio or standalone)
- Qt5 (download from qt.io) - optional for GUI client

## Building

### Linux/macOS

**Using the build script:**
```bash
./build_project.sh
```

**Using Makefile:**
```bash
make build
```

**Manual CMake:**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Windows

**Using CMake GUI:**
1. Open CMake GUI
2. Set source directory to project root
3. Set build directory to `project/build`
4. Click "Configure" and select your compiler
5. Click "Generate"
6. Open the generated solution in Visual Studio and build

**Using CMake command line:**
```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

## Running

### Start the Server

**Linux/macOS:**
```bash
./build/bin/server [host] [port]
# or
./server

# Default: 0.0.0.0:8080
```

**Windows:**
```cmd
build\bin\Release\server.exe
```

### Start the Client

**Linux/macOS:**
```bash
./build/bin/client
# or
./client
```

**Windows:**
```cmd
build\bin\Release\client.exe
```

### Quick Start with Makefile

```bash
# Terminal 1 - Run server
make run-server

# Terminal 2 - Run client
make run-client
```

## Project Structure

```
project/
├── CMakeLists.txt           # Build configuration
├── Makefile                 # Quick build commands
├── build_project.sh         # Linux build script
├── README.md               # This file
│
├── config/
│   ├── server_config.json  # Server configuration
│   └── client_config.json  # Client configuration
│
├── include/
│   ├── common.hpp          # Common definitions and utilities
│   └── message_structs.hpp # Protocol message structures
│
├── src/
│   ├── server/
│   │   ├── main.cpp        # Server entry point
│   │   ├── Server.hpp/cpp  # Main server class with I/O multiplexing
│   │   └── ClientHandler.hpp/cpp  # Per-client message handler
│   │
│   ├── client/
│   │   ├── main.cpp        # Client entry point
│   │   ├── Client.hpp/cpp  # Client networking class
│   │   └── UI.hpp/cpp      # Qt GUI implementation
│   │
│   ├── protocol/
│   │   ├── Protocol.hpp/cpp  # Message encoding/decoding
│   │   └── Network.hpp/cpp   # Socket operations wrapper
│   │
│   ├── utils/
│   │   ├── Logger.hpp/cpp    # Thread-safe logging
│   │   └── Parser.hpp/cpp    # Message payload parsing
│   │
│   └── db/
│       └── Database.hpp/cpp  # In-memory user database
│
├── logs/
│   ├── server.log          # Server logs (generated)
│   └── client.log          # Client logs (generated)
│
└── test/
    ├── test_message_format.cpp
    └── test_network_connection.cpp
```

## Protocol Design

### Message Types

| Category | Message Type | Code |
|----------|-------------|------|
| Authentication | REGISTER_REQUEST | 0x0101 |
|  | LOGIN_REQUEST | 0x0111 |
|  | LOGOUT_REQUEST | 0x0121 |
| Study | SET_LEVEL_REQUEST | 0x0201 |
|  | GET_LESSON_LIST_REQUEST | 0x0301 |
|  | GET_LESSON_CONTENT_REQUEST | 0x0311 |
| Exercises | SUBMIT_QUIZ_REQUEST | 0x0401 |
|  | SUBMIT_EXERCISE_REQUEST | 0x0411 |
| Games | GAME_START_REQUEST | 0x0501 |
|  | GAME_MOVE_REQUEST | 0x0511 |
| Communication | CHAT_MESSAGE | 0x0701 |
|  | VOICE_CALL_REQUEST | 0x0711 |
| System | HEARTBEAT_REQUEST | 0x0901 |
|  | ERROR_MESSAGE | 0x0911 |

### Wire Format

```
TYPE|LENGTH|SEQUENCE|PAYLOAD\n

Example:
273|15|1|username|password\n
```

Where:
- TYPE: Message type code (decimal)
- LENGTH: Payload length in bytes
- SEQUENCE: Message sequence number
- PAYLOAD: Message data (format varies by type)
- \n: Message delimiter

## Usage Examples

### 1. Register and Login

**Client GUI:**
1. Go to "Connection" tab → Connect to server
2. Go to "Authentication" tab
3. Enter username, password, select role
4. Click "Register"
5. Click "Login"

### 2. Study Lessons

1. Go to "Study" tab
2. Select your level → Click "Set Level"
3. Click "Refresh Lessons"
4. Select a lesson → Click "View Lesson"

### 3. Submit Exercise

1. Go to "Exercises" tab
2. Enter Quiz ID and answers
3. Click "Submit Quiz"
4. View your score

### 4. Play Game

1. Go to "Games" tab
2. Select game type → Click "Start Game"
3. Enter moves and click "Send Move"

### 5. Chat with Others

1. Go to "Communication" tab
2. Enter recipient username
3. Type message → Click "Send Message"

## Platform Support

### Cross-Platform Compatibility

The application handles platform differences automatically:

**Windows:**
- Uses Winsock2 (ws2_32.lib)
- `select()` for I/O multiplexing
- IOCP could be added for better performance

**Linux:**
- Uses POSIX sockets
- `poll()` for I/O multiplexing
- Can use `epoll()` for better scalability

**macOS:**
- Uses POSIX sockets
- `poll()` for I/O multiplexing
- Can use `kqueue()` for better performance

### Platform-Specific Macros

```cpp
#ifdef _WIN32
    // Windows-specific code
#else
    // POSIX (Linux/macOS) code
#endif
```

## Troubleshooting

### Build Issues

**Qt not found:**
```
Solution: Install Qt5 development packages or build server only
The server can be built without Qt
```

**CMake version too old:**
```
Solution: Update CMake to 3.10 or higher
```

### Runtime Issues

**Connection refused:**
```
Solution: 
1. Check if server is running
2. Verify firewall settings
3. Check server address and port
```

**Login failed:**
```
Solution:
1. Register first if new user
2. Check credentials
3. Verify server logs for errors
```

**Socket errors on Windows:**
```
Solution: Ensure Winsock is initialized (automatic in this implementation)
```

### Logs

Check application logs for debugging:
- Server: `logs/server.log`
- Client: `logs/client.log`

## Default Accounts

The system creates default accounts on first run:

| Username | Password | Role |
|----------|----------|------|
| admin | admin123 | Admin |
| teacher1 | teacher123 | Teacher |

## Performance Notes

- **Server can handle**: 100+ concurrent clients (configurable)
- **I/O multiplexing**: Efficient single-thread design
- **Memory**: ~1MB per connected client
- **Latency**: <10ms on localhost, varies on network

## Security Notes

⚠️ **This is a demonstration project. For production use:**
- Use TLS/SSL for encryption
- Implement proper password hashing (bcrypt, scrypt)
- Add input sanitization and validation
- Implement rate limiting
- Add authentication tokens instead of password re-transmission
- Use prepared statements if adding SQL database

## License

This project is provided for educational purposes.

## Authors

Built as a comprehensive C++ network programming demonstration following professional software engineering practices.

## Contributing

This is an educational project demonstrating:
- C++17 modern practices
- POSIX socket programming
- I/O multiplexing patterns
- Custom protocol design
- Cross-platform development
- Qt GUI development
- CMake build systems

---

**For questions or issues, check the logs first, then review the protocol documentation.**

# Elearning-cpp
