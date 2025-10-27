# English Learning Platform - Quick Reference Guide

## Message Type Quick Reference

### Authentication Messages (0x01xx)

| Code | Type | Direction | Payload | Example |
|------|------|-----------|---------|---------|
| 257 | REGISTER_REQUEST | C→S | `username\|password\|role` | `257\|15\|1\|john\|pass123\|0\n` |
| 258 | REGISTER_SUCCESS | S→C | `message` | `258\|26\|1\|Registration successful\n` |
| 259 | REGISTER_FAILED | S→C | `error` | `259\|23\|1\|Username already exists\n` |
| 273 | LOGIN_REQUEST | C→S | `username\|password` | `273\|13\|2\|john\|pass123\n` |
| 274 | LOGIN_SUCCESS | S→C | `role\|level\|score` | `274\|5\|2\|0\|0\|100\n` |
| 275 | LOGIN_FAILED | S→C | `error` | `275\|20\|2\|Invalid credentials\n` |
| 289 | LOGOUT_REQUEST | C→S | (empty) | `289\|0\|3\|\n` |
| 290 | LOGOUT_SUCCESS | S→C | `message` | `290\|19\|3\|Logged out successfully\n` |

### Study Messages (0x02xx)

| Code | Type | Direction | Payload | Example |
|------|------|-----------|---------|---------|
| 513 | SET_LEVEL_REQUEST | C→S | `level` (0/1/2) | `513\|1\|4\|1\n` |
| 514 | SET_LEVEL_SUCCESS | S→C | `message` | `514\|21\|4\|Level set to INTERMEDIATE\n` |

### Content Messages (0x03xx)

| Code | Type | Direction | Payload | Example |
|------|------|-----------|---------|---------|
| 769 | GET_LESSON_LIST_REQUEST | C→S | (empty) | `769\|0\|5\|\n` |
| 770 | GET_LESSON_LIST_RESPONSE | S→C | `lesson1;lesson2;...` | `770\|45\|5\|lesson_b1:Greetings;lesson_b2:Numbers\n` |
| 785 | GET_LESSON_CONTENT_REQUEST | C→S | `lessonId` | `785\|10\|6\|lesson_b1\n` |
| 786 | GET_LESSON_CONTENT_RESPONSE | S→C | `content` | `786\|120\|6\|Video: url\nAudio: url\nText: ...\n` |

### Exercise Messages (0x04xx)

| Code | Type | Direction | Payload | Example |
|------|------|-----------|---------|---------|
| 1025 | SUBMIT_QUIZ_REQUEST | C→S | `quizId\|answers` | `1025\|12\|7\|quiz_1\|A,B,C,A\n` |
| 1026 | SUBMIT_QUIZ_RESPONSE | S→C | `score\|feedback` | `1026\|18\|7\|85\|Great job!\n` |
| 1041 | SUBMIT_EXERCISE_REQUEST | C→S | `exerciseId\|content` | `1041\|30\|8\|ex_1\|My essay content...\n` |
| 1042 | SUBMIT_EXERCISE_RESPONSE | S→C | `score\|feedback` | `1042\|20\|8\|90\|Well written!\n` |

### Game Messages (0x05xx)

| Code | Type | Direction | Payload | Example |
|------|------|-----------|---------|---------|
| 1281 | GAME_START_REQUEST | C→S | `gameType` | `1281\|14\|9\|Word Matching\n` |
| 1282 | GAME_START_RESPONSE | S→C | `sessionId\|items` | `1282\|50\|9\|sess_123\|cat=animal;dog=pet\n` |
| 1297 | GAME_MOVE_REQUEST | C→S | `moveData` | `1297\|10\|10\|cat=animal\n` |
| 1298 | GAME_MOVE_RESPONSE | S→C | `result\|score` | `1298\|11\|10\|correct\|10\n` |
| 1313 | GAME_END_NOTIFICATION | S→C | `finalScore` | `1313\|3\|11\|100\n` |

### Assessment Messages (0x06xx)

| Code | Type | Direction | Payload | Example |
|------|------|-----------|---------|---------|
| 1537 | GET_SCORE_REQUEST | C→S | (empty) | `1537\|0\|12\|\n` |
| 1538 | GET_SCORE_RESPONSE | S→C | `score` | `1538\|3\|12\|250\n` |
| 1553 | GET_FEEDBACK_REQUEST | C→S | (empty) | `1553\|0\|13\|\n` |
| 1554 | GET_FEEDBACK_RESPONSE | S→C | `feedback1;feedback2` | `1554\|40\|13\|Good work;Keep practicing\n` |
| 1569 | SEND_FEEDBACK_REQUEST | C→S | `student\|exerciseId\|feedback` | `1569\|30\|14\|john\|ex_1\|Good work!\n` |
| 1570 | SEND_FEEDBACK_SUCCESS | S→C | `message` | `1570\|17\|14\|Feedback sent\n` |

### Communication Messages (0x07xx)

| Code | Type | Direction | Payload | Example |
|------|------|-----------|---------|---------|
| 1793 | CHAT_MESSAGE | C→S or S→C | `recipient\|message` | `1793\|20\|15\|bob\|Hello there!\n` |
| 1794 | CHAT_MESSAGE_ACK | S→C | `message` | `1794\|17\|15\|Message delivered\n` |
| 1809 | VOICE_CALL_REQUEST | C→S | `targetUser` | `1809\|8\|16\|teacher1\n` |
| 1810 | VOICE_CALL_ACCEPT | S→C | `message` | `1810\|13\|16\|Call accepted\n` |
| 1811 | VOICE_CALL_REJECT | S→C | `message` | `1811\|13\|16\|Call rejected\n` |
| 1812 | VOICE_CALL_END | C→S or S→C | (empty) | `1812\|0\|17\|\n` |

### Admin Messages (0x08xx)

| Code | Type | Direction | Payload | Example |
|------|------|-----------|---------|---------|
| 2049 | ADD_GAME_ITEM_REQUEST | C→S | `gameType\|item` | `2049\|25\|18\|Word Matching\|cat=animal\n` |
| 2050 | ADD_GAME_ITEM_SUCCESS | S→C | `message` | `2050\|15\|18\|Item added\n` |
| 2051 | ADD_GAME_ITEM_FAILED | S→C | `error` | `2051\|15\|18\|Failed to add\n` |

### System Messages (0x09xx)

| Code | Type | Direction | Payload | Example |
|------|------|-----------|---------|---------|
| 2305 | HEARTBEAT_REQUEST | C→S | (empty) | `2305\|0\|100\|\n` |
| 2306 | HEARTBEAT_RESPONSE | S→C | (empty) | `2306\|0\|100\|\n` |
| 2321 | ERROR_MESSAGE | S→C | `errorCode\|description` | `2321\|25\|101\|5\|Not authenticated\n` |
| 2337 | DISCONNECT_NOTIFICATION | S→C | `message` | `2337\|14\|102\|Server closing\n` |

---

## Error Codes

| Code | Name | Description |
|------|------|-------------|
| 0 | SUCCESS | Operation successful |
| 1 | INVALID_FORMAT | Message format is invalid |
| 2 | INVALID_CREDENTIALS | Username or password incorrect |
| 3 | USER_ALREADY_EXISTS | Registration failed, username taken |
| 4 | USER_NOT_FOUND | User does not exist |
| 5 | NOT_AUTHENTICATED | Login required for this operation |
| 6 | PERMISSION_DENIED | User role lacks permission |
| 7 | RESOURCE_NOT_FOUND | Requested resource not found |
| 8 | INTERNAL_ERROR | Server internal error |
| 9 | DATABASE_ERROR | Database operation failed |
| 10 | INVALID_PARAMETER | Invalid parameter provided |

---

## User Roles

| Code | Role | Description |
|------|------|-------------|
| 0 | STUDENT | Regular user, can study and play games |
| 1 | TEACHER | Can provide feedback to students |
| 2 | ADMIN | Can manage game items and view logs |

---

## Proficiency Levels

| Code | Level | Description |
|------|-------|-------------|
| 0 | BEGINNER | Basic English learners |
| 1 | INTERMEDIATE | Mid-level English learners |
| 2 | ADVANCED | Advanced English learners |

---

## Connection States

| State | Description |
|-------|-------------|
| DISCONNECTED | No connection |
| CONNECTING | TCP handshake in progress |
| CONNECTED | TCP connected, not authenticated |
| AUTHENTICATED | Logged in, full access |

---

## Common Message Sequences

### 1. Complete User Workflow

```
1. REGISTER_REQUEST → REGISTER_SUCCESS
2. LOGIN_REQUEST → LOGIN_SUCCESS
3. SET_LEVEL_REQUEST → SET_LEVEL_SUCCESS
4. GET_LESSON_LIST_REQUEST → GET_LESSON_LIST_RESPONSE
5. GET_LESSON_CONTENT_REQUEST → GET_LESSON_CONTENT_RESPONSE
6. SUBMIT_QUIZ_REQUEST → SUBMIT_QUIZ_RESPONSE
7. GAME_START_REQUEST → GAME_START_RESPONSE
8. GAME_MOVE_REQUEST → GAME_MOVE_RESPONSE (multiple times)
9. GET_SCORE_REQUEST → GET_SCORE_RESPONSE
10. LOGOUT_REQUEST → LOGOUT_SUCCESS
```

### 2. Minimal Session

```
1. LOGIN_REQUEST → LOGIN_SUCCESS
2. [Perform operations]
3. HEARTBEAT_REQUEST → HEARTBEAT_RESPONSE (every 30s)
4. LOGOUT_REQUEST → LOGOUT_SUCCESS
```

### 3. Chat Between Users

```
Client A → Server: CHAT_MESSAGE (recipient: bob, message: "Hi")
Server → Client B: CHAT_MESSAGE (sender: alice, message: "Hi")
Server → Client A: CHAT_MESSAGE_ACK
```

---

## Server Configuration

**File**: `config/server_config.json`

```json
{
    "host": "0.0.0.0",
    "port": 8080,
    "max_clients": 100,
    "timeout_seconds": 300
}
```

- **host**: `0.0.0.0` listens on all interfaces, `127.0.0.1` for localhost only
- **port**: Default 8080, ensure firewall allows this port
- **max_clients**: Maximum concurrent connections
- **timeout_seconds**: Session timeout (default 300 = 5 minutes)

---

## Client Configuration

**File**: `config/client_config.json`

```json
{
    "server_host": "127.0.0.1",
    "server_port": 8080,
    "heartbeat_interval": 30
}
```

- **server_host**: Server IP address
- **server_port**: Server port
- **heartbeat_interval**: Seconds between heartbeat messages (default 30)

---

## Command-Line Usage

### Server

```bash
# Default (0.0.0.0:8080)
./build/bin/server

# Custom host and port
./build/bin/server 192.168.1.100 9000
```

### Qt GUI Client

```bash
./build/bin/client
```

Then use the GUI to:
1. Go to "Connection" tab → Connect
2. Go to "Authentication" tab → Register/Login
3. Use other tabs for study, exercises, games, etc.

### Console Client

```bash
./console_client
```

Commands:
- `connect <host> <port>` - Connect to server
- `register <username> <password> <role>` - Register new user
- `login <username> <password>` - Login
- `setlevel <level>` - Set proficiency level (0/1/2)
- `lessons` - Get lesson list
- `lesson <id>` - Get lesson content
- `quiz <id> <answers>` - Submit quiz (answers comma-separated)
- `game <type>` - Start game
- `move <data>` - Send game move
- `chat <recipient> <message>` - Send chat message
- `score` - Get your score
- `feedback` - Get feedback
- `logout` - Logout
- `quit` - Disconnect and exit

---

## File Structure

```
project/
├── build/                     # Build output directory
│   └── bin/
│       ├── server             # Server executable
│       └── client             # Qt client executable
├── config/
│   ├── server_config.json     # Server configuration
│   └── client_config.json     # Client configuration
├── include/
│   ├── common.hpp             # Common definitions
│   └── message_structs.hpp    # Message types and structures
├── src/
│   ├── server/
│   │   ├── Server.cpp/hpp     # Main server with I/O mux
│   │   ├── ClientHandler.cpp/hpp  # Per-client handler
│   │   └── main.cpp           # Server entry point
│   ├── client/
│   │   ├── Client.cpp/hpp     # Client networking
│   │   ├── UI.cpp/hpp         # Qt GUI
│   │   └── main.cpp           # Client entry point
│   ├── protocol/
│   │   ├── Protocol.cpp/hpp   # Message encoding/decoding
│   │   └── Network.cpp/hpp    # Socket operations
│   ├── utils/
│   │   ├── Logger.cpp/hpp     # Logging
│   │   └── Parser.cpp/hpp     # String parsing
│   └── db/
│       └── Database.cpp/hpp   # In-memory database
├── logs/
│   ├── server.log             # Server logs
│   └── client.log             # Client logs
├── docs/
│   └── [documentation files]
├── CMakeLists.txt             # Build configuration
├── Makefile                   # Quick build commands
└── README.md                  # Main documentation
```

---

## Build Commands

### Linux/macOS

```bash
# Using build script
./build_project.sh

# Using Makefile
make build

# Manual CMake
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Windows

```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

---

## Debugging

### Enable Debug Logging

Edit `src/utils/Logger.cpp` and set log level:

```cpp
Logger::getInstance().setLevel(LogLevel::DEBUG);
```

### View Logs

```bash
# Server logs
tail -f logs/server.log

# Client logs
tail -f logs/client.log
```

### Log Format

```
[YYYY-MM-DD HH:MM:SS] [LEVEL] Message
```

Example:
```
[2025-10-27 10:15:23] [INFO] Server started on 0.0.0.0:8080
[2025-10-27 10:15:25] [INFO] Client connected: 192.168.1.100:54321
[2025-10-27 10:15:30] [DEBUG] Received: LOGIN_REQUEST from john@192.168.1.100:54321
```

---

## Troubleshooting

### Connection Refused

**Problem**: Client cannot connect to server

**Solutions**:
1. Check server is running: `ps aux | grep server`
2. Check server is listening: `netstat -an | grep 8080`
3. Check firewall: `sudo ufw allow 8080` (Linux) or Windows Firewall settings
4. Verify server address: Use `ifconfig` (Linux/Mac) or `ipconfig` (Windows)

### Login Failed

**Problem**: Invalid credentials error

**Solutions**:
1. Register first if new user
2. Check username/password are correct
3. Check server logs for detailed error
4. Verify database is initialized

### Timeout

**Problem**: Client disconnected after inactivity

**Solutions**:
1. Increase timeout in `config/server_config.json`
2. Ensure heartbeat is working (default: every 30s)
3. Check network stability

### Build Errors

**Problem**: Compilation fails

**Solutions**:
1. Check CMake version: `cmake --version` (need 3.10+)
2. Check compiler: `g++ --version` or `clang --version`
3. Install Qt5 for GUI: `sudo apt-get install qtbase5-dev`
4. Clean build: `rm -rf build && mkdir build`

---

## Performance Tuning

### Server

- **Max Clients**: Increase `max_clients` in config (default: 100)
- **Timeout**: Adjust `timeout_seconds` based on use case
- **I/O Multiplexing**: Linux uses `poll()`, can be upgraded to `epoll()` for better scalability
- **Database**: Current in-memory, consider persistent storage for production

### Client

- **Heartbeat**: Adjust `heartbeat_interval` (default: 30s)
- **Buffer Size**: Increase if sending large messages (default: 4096 bytes)

---

## Security Checklist

⚠️ **Current implementation is for educational purposes**

For production deployment:

- [ ] Enable TLS/SSL encryption
- [ ] Use bcrypt/scrypt for password hashing
- [ ] Implement rate limiting
- [ ] Add input sanitization
- [ ] Use authentication tokens (JWT)
- [ ] Enable CSRF protection
- [ ] Add audit logging
- [ ] Implement role-based access control (RBAC)
- [ ] Regular security audits
- [ ] Keep dependencies updated

---

## Default Accounts

The server creates these accounts on first run:

| Username | Password | Role |
|----------|----------|------|
| admin | admin123 | ADMIN |
| teacher1 | teacher123 | TEACHER |

⚠️ Change these passwords in production!

---

## Useful Testing Commands

### Test Server Connectivity

```bash
# Test if port is open
nc -zv 127.0.0.1 8080

# Or using telnet
telnet 127.0.0.1 8080
```

### Send Raw Message (for debugging)

```bash
# Using netcat
echo "273|13|1|john|pass123" | nc 127.0.0.1 8080
```

### Monitor Network Traffic

```bash
# Capture packets on port 8080
sudo tcpdump -i any port 8080 -A

# Or using Wireshark with filter: tcp.port == 8080
```

---

## API Summary (Client.hpp)

### Connection
- `bool connect(host, port)` - Connect to server
- `void disconnect()` - Disconnect from server
- `bool isConnected()` - Check connection status

### Authentication
- `bool registerUser(username, password, role)` - Register new account
- `bool login(username, password, userData)` - Login and get user data
- `bool logout()` - Logout from server

### Study
- `bool setLevel(level)` - Set proficiency level
- `vector<string> getLessonList()` - Get available lessons
- `string getLessonContent(lessonId)` - Get lesson details

### Exercises
- `bool submitQuiz(quizId, answers, score)` - Submit quiz
- `bool submitExercise(exerciseId, content, score)` - Submit exercise

### Games
- `string startGame(gameType)` - Start a game
- `bool sendGameMove(moveData, response)` - Send game move

### Communication
- `bool sendChatMessage(recipient, message)` - Send chat
- `bool initiateVoiceCall(targetUser)` - Start voice call

### Assessment
- `int getScore()` - Get total score
- `vector<string> getFeedback()` - Get feedback list

### System
- `bool sendHeartbeat()` - Send keep-alive

---

## Contact & Support

For issues or questions:
1. Check logs first: `logs/server.log` or `logs/client.log`
2. Review documentation: `README.md`, `TECHNICAL_DOCUMENTATION.md`
3. Check troubleshooting section above

---

**Quick Reference Version**: 1.0  
**Last Updated**: October 27, 2025  
**For Full Documentation**: See TECHNICAL_DOCUMENTATION.md and ARCHITECTURE_DIAGRAMS.md

