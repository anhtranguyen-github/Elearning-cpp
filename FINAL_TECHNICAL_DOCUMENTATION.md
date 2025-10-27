# English Learning Platform - Final Technical Documentation

## Table of Contents
1. [System Architecture Overview](#1-system-architecture-overview)
2. [Communication Diagrams](#2-communication-diagrams)
3. [Message Design](#3-message-design)
4. [Message Formats](#4-message-formats)

---

## 1. System Architecture Overview

### 1.1 Hybrid TCP/UDP Client-Server System

```
┌─────────────────────────────────────────────────────────────────────┐
│              ENGLISH LEARNING PLATFORM ARCHITECTURE                  │
│                    (Hybrid TCP + UDP Protocol)                       │
└─────────────────────────────────────────────────────────────────────┘

┌──────────────────────┐                          ┌──────────────────────┐
│   CLIENT SIDE        │                          │    SERVER SIDE       │
│                      │                          │                      │
│  ┌────────────────┐  │                          │  ┌────────────────┐  │
│  │   Qt GUI/      │  │                          │  │   Main Server  │  │
│  │   Console UI   │  │                          │  │   Process      │  │
│  └────────┬───────┘  │                          │  └────────┬───────┘  │
│           │          │                          │           │          │
│           v          │                          │           v          │
│  ┌────────────────┐  │  ──── TCP (Port 8080) ──►│  ┌────────────────┐  │
│  │ Client Logic   │  │  • Authentication        │  │  TCP Handler   │  │
│  │ • Auth         │◄─┼  • Text Chat            │  │  (select/poll) │  │
│  │ • Chat         │  │  • Session Mgmt          │  └────────┬───────┘  │
│  │ • Commands     │  │  • Heartbeat             │           │          │
│  └────────┬───────┘  │                          │           │          │
│           │          │  ──── UDP (Port 8081) ──►│  ┌────────▼───────┐  │
│  ┌────────▼───────┐  │  • Games                 │  │  UDP Handler   │  │
│  │ UDP Client     │  │  • Content Stream        │  │  (recvfrom)    │  │
│  │ • Games        │◄─┼  • Voice Files          │  └────────┬───────┘  │
│  │ • Content      │  │  • File Transfer         │           │          │
│  │ • Files        │  │                          │           v          │
│  └────────┬───────┘  │                          │  ┌────────────────┐  │
│           │          │                          │  │   Database &   │  │
│           v          │                          │  │  File Storage  │  │
│  ┌────────────────┐  │                          │  │                │  │
│  │  Token Store   │  │                          │  │ • Users        │  │
│  │  File Cache    │  │                          │  │ • Sessions     │  │
│  └────────────────┘  │                          │  │ • Voice Files  │  │
│                      │                          │  │ • Lessons      │  │
└──────────────────────┘                          └──────────────────────┘
```

### 1.2 Protocol Usage

```
┌────────────────────────────────────────────────────────────────────┐
│                       PROTOCOL DISTRIBUTION                         │
└────────────────────────────────────────────────────────────────────┘

TCP (Reliable, Connection-Oriented, Port 8080)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✓ Authentication
  - Register (username, password, role)
  - Login → Receive JWT Token
  - Logout
  - Token Refresh

✓ Session Management
  - Heartbeat (every 30s)
  - Session timeout detection
  - Connection state tracking

✓ Text Communication
  - Chat messages (text only)
  - Voice file notifications
  - Message acknowledgments

✓ Administrative
  - Add game items
  - System commands
  - Error messages


UDP (Fast, Connectionless, Port 8081)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✓ Games
  - Game moves (real-time)
  - Move responses
  - Score updates

✓ Content Streaming
  - Lesson videos (chunked)
  - Audio lessons (chunked)
  - Content metadata

✓ File Transfer
  - Voice chat files (.wav, .mp3)
  - Voice file chunks
  - File download requests
  - Lesson materials


Why Hybrid?
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
TCP: Reliability critical for auth, chat, session
UDP: Speed critical for games, streaming, files
```

### 1.3 Component Layers

```
┌────────────────────────────────────────────────────────────────────┐
│                      LAYERED ARCHITECTURE                           │
└────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────────┐
│  PRESENTATION LAYER                                                 │
│  ┌──────────────────┐          ┌──────────────────┐                │
│  │  Qt GUI Client   │          │ Console Client   │                │
│  │  • Auth UI       │          │ • Text Interface │                │
│  │  • Chat UI       │          │ • Commands       │                │
│  │  • Study UI      │          │ • Menu System    │                │
│  │  • Game UI       │          │                  │                │
│  └──────────────────┘          └──────────────────┘                │
└────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────────┐
│  APPLICATION LAYER                                                  │
│  ┌──────────────────┐          ┌──────────────────┐                │
│  │  Client          │          │  Server          │                │
│  │  • TCP Socket    │          │  • TCP Listener  │                │
│  │  • UDP Socket    │          │  • UDP Listener  │                │
│  │  • Token Mgmt    │          │  • Token Verify  │                │
│  │  • File Cache    │          │  • File Store    │                │
│  └──────────────────┘          └──────────────────┘                │
│  ┌──────────────────┐          ┌──────────────────┐                │
│  │  Protocol        │          │  ClientHandler   │                │
│  │  • TCP Parser    │          │  • Auth Handler  │                │
│  │  • UDP Parser    │          │  • Game Handler  │                │
│  │  • Token Parser  │          │  • File Handler  │                │
│  └──────────────────┘          └──────────────────┘                │
└────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────────┐
│  DATA LAYER                                                         │
│  ┌──────────────────┐          ┌──────────────────┐                │
│  │  Database        │          │  File System     │                │
│  │  • Users         │          │  • Voice Files   │                │
│  │  • Sessions      │          │  • Lessons       │                │
│  │  • Tokens        │          │  • Temp Cache    │                │
│  │  • Scores        │          │  • Logs          │                │
│  └──────────────────┘          └──────────────────┘                │
└────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────────┐
│  NETWORK LAYER                                                      │
│  ┌──────────────────┐          ┌──────────────────┐                │
│  │  TCP Sockets     │          │  UDP Sockets     │                │
│  │  • Reliable      │          │  • Fast          │                │
│  │  • Ordered       │          │  • Unordered     │                │
│  │  • Stream        │          │  • Datagram      │                │
│  └──────────────────┘          └──────────────────┘                │
└────────────────────────────────────────────────────────────────────┘
```

### 1.4 Token-Based Authentication Flow

```
┌────────────────────────────────────────────────────────────────────┐
│                    JWT TOKEN AUTHENTICATION                         │
└────────────────────────────────────────────────────────────────────┘

Client                          Server                      Database
  │                               │                            │
  │ 1. LOGIN_REQUEST (TCP)        │                            │
  │    username|password           │                            │
  │ ─────────────────────────────►│                            │
  │                               │  Verify credentials        │
  │                               │ ──────────────────────────►│
  │                               │  ✓ Valid                   │
  │                               │ ◄──────────────────────────│
  │                               │                            │
  │                               │  Generate JWT Token:       │
  │                               │  {                         │
  │                               │    "user": "john",         │
  │                               │    "role": "STUDENT",      │
  │                               │    "iat": 1698765432,      │
  │                               │    "exp": 1698769032       │
  │                               │  }                         │
  │                               │  Sign with secret key      │
  │                               │                            │
  │ 2. LOGIN_SUCCESS (TCP)        │                            │
  │    token|role|level|score     │                            │
  │ ◄─────────────────────────────│                            │
  │                               │                            │
  │  Store token locally:          │                            │
  │  authToken = "eyJhbGc..."     │                            │
  │                               │                            │
  │ 3. Any Request (TCP/UDP)      │                            │
  │    token|request_data         │                            │
  │ ─────────────────────────────►│                            │
  │                               │  Verify token signature    │
  │                               │  Check expiration          │
  │                               │  Extract user info         │
  │                               │  ✓ Valid                   │
  │                               │                            │
  │ 4. Response                   │                            │
  │ ◄─────────────────────────────│                            │
  │                               │                            │

Token Benefits:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✓ No password re-transmission after login
✓ Stateless verification (no database lookup)
✓ Automatic expiration (1 hour default)
✓ Can be revoked server-side if needed
✓ Contains user info (no extra queries)
```

---

## 2. Communication Diagrams

### 2.1 Basic Client-Server Communication

```
┌────────┐                                          ┌────────┐
│ Client │                                          │ Server │
└───┬────┘                                          └───┬────┘
    │                                                   │
    │  1. TCP Connection (3-way handshake)              │
    │ ──────────────────────────────────────────────►   │
    │                     SYN                            │
    │   ◄──────────────────────────────────────────────│
    │                   SYN-ACK                          │
    │ ──────────────────────────────────────────────►   │
    │                     ACK                            │
    │                                                   │
    │  2. Login with Credentials (TCP)                  │
    │ ──────────────────────────────────────────────►   │
    │     273|13|1|john|pass123                         │
    │                                                   │
    │  3. Receive Token (TCP)                           │
    │   ◄──────────────────────────────────────────────│
    │     274|65|1|eyJhbGc...xyz|0|1|250                │
    │                                                   │
    │  4. Authenticated Request with Token (TCP)        │
    │ ──────────────────────────────────────────────►   │
    │     769|45|2|eyJhbGc...xyz                        │
    │                                                   │
    │  5. Server Response (TCP)                         │
    │   ◄──────────────────────────────────────────────│
    │     770|50|2|lesson_b1:Greetings;...              │
    │                                                   │
    │  6. UDP Request (Games/Files)                     │
    │ ══════════════════════════════════════════════►   │
    │     1297|sessionId|move_data                      │
    │                                                   │
    │  7. UDP Response                                  │
    │   ◄══════════════════════════════════════════════│
    │     1298|correct|10                               │
    │                                                   │
    │  8. Heartbeat (every 30s, TCP)                    │
    │ ──────────────────────────────────────────────►   │
    │     2305|45|100|eyJhbGc...xyz                     │
    │   ◄──────────────────────────────────────────────│
    │     2306|0|100                                    │
    │                                                   │
```

### 2.2 Multi-Client Server Architecture

```
┌──────────┐     ┌──────────┐     ┌──────────┐
│ Client 1 │     │ Client 2 │     │ Client N │
└────┬─────┘     └────┬─────┘     └────┬─────┘
     │                │                │
     │ TCP LOGIN      │ TCP LOGIN      │ TCP LOGIN
     │ + Token        │ + Token        │ + Token
     │                │                │
     └────────────────┼────────────────┘
                      │
              ┌───────▼────────┐
              │  Server        │
              │                │
              │  TCP Listener  │
              │  (Port 8080)   │
              │  select()/poll │
              │                │
              │  UDP Listener  │
              │  (Port 8081)   │
              │  recvfrom()    │
              └───────┬────────┘
                      │
          ┌───────────┼───────────┐
          │           │           │
    ┌─────▼─────┐ ┌──▼──────┐ ┌──▼──────┐
    │ Handler 1 │ │Handler 2│ │Handler N│
    │ + Token   │ │+ Token  │ │+ Token  │
    │   Verify  │ │  Verify │ │  Verify │
    └─────┬─────┘ └──┬──────┘ └──┬──────┘
          │           │           │
          └───────────┼───────────┘
                      │
              ┌───────▼────────┐
              │   Database     │
              │   + Tokens     │
              │   + Files      │
              └────────────────┘
```

### 2.3 Voice Chat File Transfer Communication

```
┌──────────┐                  ┌────────┐                  ┌──────────┐
│ Client A │                  │ Server │                  │ Client B │
│ (Sender) │                  │        │                  │(Receiver)│
└────┬─────┘                  └───┬────┘                  └────┬─────┘
     │                            │                            │
     │ 1. Record Voice            │                            │
     │    (save as .wav)          │                            │
     │                            │                            │
     │ 2. VOICE_FILE_UPLOAD (UDP) │                            │
     │    token|fileId|chunk_0    │                            │
     │ ═══════════════════════════►                            │
     │                            │ Store chunk_0              │
     │                            │                            │
     │ 3. VOICE_FILE_UPLOAD (UDP) │                            │
     │    token|fileId|chunk_1    │                            │
     │ ═══════════════════════════►                            │
     │                            │ Store chunk_1              │
     │                            │ ...                        │
     │                            │ All chunks received        │
     │                            │ Reassemble file:           │
     │                            │ voice_12345.wav            │
     │                            │                            │
     │ 4. VOICE_MESSAGE_ACK (TCP) │                            │
     │ ◄───────────────────────────                            │
     │    fileId|size|success     │                            │
     │                            │                            │
     │                            │ 5. VOICE_CHAT_NOTIFY (TCP) │
     │                            │    sender:A|fileId|size    │
     │                            │ ───────────────────────────►
     │                            │                            │
     │                            │ 6. VOICE_FILE_REQUEST (UDP)│
     │                            │    token|fileId            │
     │                            │ ◄═══════════════════════════
     │                            │                            │
     │                            │ 7. VOICE_FILE_DATA (UDP)   │
     │                            │    fileId|chunk_0          │
     │                            │ ═══════════════════════════►
     │                            │    fileId|chunk_1          │
     │                            │ ═══════════════════════════►
     │                            │    ... (all chunks)        │
     │                            │                            │
     │                            │ 8. Play Voice File         │
     │                            │                            │
     │                            │ 9. VOICE_FILE_ACK (TCP)    │
     │                            │ ◄───────────────────────────
     │                            │    received                │
```

### 2.4 Game Communication (UDP for Moves)

```
Client                          Server (TCP:8080, UDP:8081)
  │                                     │
  │ 1. GAME_START_REQUEST (TCP)        │
  │    token|Word Matching              │
  │ ───────────────────────────────────►│
  │                                     │ Create session
  │ 2. GAME_START_RESPONSE (TCP)       │ Load items
  │    sessionId|cat=animal;dog=pet...  │
  │ ◄───────────────────────────────────│
  │                                     │
  │ 3. GAME_MOVE (UDP) - Fast!         │
  │    sessionId|cat=animal             │
  │ ═══════════════════════════════════►│
  │                                     │ Validate
  │ 4. MOVE_RESULT (UDP) - Fast!       │ Score
  │    correct|score:10                 │
  │ ◄═══════════════════════════════════│
  │                                     │
  │ 5. GAME_MOVE (UDP)                 │
  │    sessionId|dog=cat                │
  │ ═══════════════════════════════════►│
  │                                     │
  │ 6. MOVE_RESULT (UDP)               │
  │    incorrect|score:0                │
  │ ◄═══════════════════════════════════│
  │                                     │
  │ ... (multiple fast UDP exchanges)   │
  │                                     │
  │ 7. GAME_END (TCP) - Reliable       │
  │    token|sessionId                  │
  │ ───────────────────────────────────►│
  │                                     │ Save score
  │ 8. FINAL_SCORE (TCP)               │
  │    totalScore:100                   │
  │ ◄───────────────────────────────────│
```

### 2.5 Content Streaming (UDP for Speed)

```
Client                          Server (TCP:8080, UDP:8081)
  │                                     │
  │ 1. GET_LESSON_LIST (TCP)           │
  │    token                            │
  │ ───────────────────────────────────►│
  │                                     │ Query lessons
  │ 2. LESSON_LIST (TCP)               │ by level
  │    lesson_b1:Greetings;...          │
  │ ◄───────────────────────────────────│
  │                                     │
  │ 3. GET_LESSON_CONTENT (UDP)        │
  │    token|lesson_b1                  │
  │ ═══════════════════════════════════►│
  │                                     │ Prepare
  │ 4. LESSON_METADATA (TCP)           │ content
  │    chunks:150|size:4.5MB|type:mp4   │
  │ ◄───────────────────────────────────│
  │                                     │
  │ 5. CONTENT_CHUNK (UDP) - Fast!     │
  │    lesson_b1|chunk_0|[video_data]   │
  │ ◄═══════════════════════════════════│
  │                                     │
  │ 6. CONTENT_CHUNK (UDP)             │
  │    lesson_b1|chunk_1|[video_data]   │
  │ ◄═══════════════════════════════════│
  │                                     │
  │ ... (streaming chunks via UDP)      │
  │                                     │
  │ 7. CONTENT_COMPLETE (TCP)          │
  │    lesson_b1|success                │
  │ ◄───────────────────────────────────│
  │                                     │
  │ 8. Display video/audio             │
```

---

## 3. Message Design

### 3.1 Design Principles

The protocol follows these key principles:

1. **Token-Based Security**
   - JWT tokens for authentication
   - Tokens in every authenticated request
   - No password re-transmission

2. **Hybrid Protocol**
   - TCP for reliability (auth, chat, session)
   - UDP for speed (games, streaming, files)

3. **Clear Structure**
   - Header + Payload architecture
   - Consistent message format

4. **Type Safety**
   - Strongly-typed message codes
   - Hex prefixes by category

5. **Framing**
   - TCP: Newline delimiter (\n)
   - UDP: Datagram boundaries

6. **Stateful with Tokens**
   - Server maintains sessions
   - Tokens carry user identity

### 3.2 Message Categories

Messages organized by hex prefix:

| Prefix | Category | Protocol | Examples |
|--------|----------|----------|----------|
| 0x01xx | Authentication | TCP | LOGIN, REGISTER, LOGOUT|
| 0x02xx | Study Setup | TCP | SET_LEVEL |
| 0x03xx | Content Access | UDP | GET_LESSON_LIST, GET_LESSON_CONTENT, CONTENT_CHUNK |
| 0x04xx | Exercises | TCP | SUBMIT_QUIZ, SUBMIT_EXERCISE |
| 0x05xx | Games | UDP | GAME_START, GAME_MOVE, GAME_END |
| 0x06xx | Assessment | TCP | GET_SCORE, GET_FEEDBACK, SEND_FEEDBACK |
| 0x07xx | Communication | TCP+UDP | CHAT(TCP), VOICE_CHAT_FILE(UDP) |
| 0x08xx | Admin | TCP | ADD_GAME_ITEM |
| 0x09xx | System | TCP | HEARTBEAT, ERROR, DISCONNECT |


### 3.4 Request-Response Pattern

Most messages follow request-response:

**TCP Messages:**
- Request: Client → Server (odd codes: 0x0101, 0x0111)
- Success: Server → Client (even codes: 0x0102, 0x0112)
- Failure: Server → Client (alt codes: 0x0103, 0x0113)

**UDP Messages:**
- Request: Client → Server (any code)
- Response: Server → Client (immediate)
- No failure codes (timeout = failure)

**Examples:**
- LOGIN_REQUEST (TCP 0x0111) → LOGIN_SUCCESS (TCP 0x0112) or LOGIN_FAILED (TCP 0x0113)
- GAME_MOVE (UDP 0x0511) → MOVE_RESULT (UDP 0x0512)
- VOICE_FILE_UPLOAD (UDP 0x0715) → VOICE_MESSAGE_ACK (TCP 0x0716)

---

## 4. Message Formats

### 4.1 TCP Wire Format

```
TYPE|LENGTH|SEQUENCE|PAYLOAD\n
```

**Fields:**
- **TYPE**: Message type code (decimal, e.g., 273 for LOGIN_REQUEST)
- **LENGTH**: Payload length in bytes
- **SEQUENCE**: Message sequence number
- **PAYLOAD**: Message-specific data
- **\n**: Message delimiter (newline)

**Example:**
```
274|65|1|eyJhbGciOiJIUzI1NiJ9.eyJ1c2VyIjoiam9obiJ9.abc123|0|1|250\n
```
- TYPE: 274 (LOGIN_SUCCESS)
- LENGTH: 65 bytes
- SEQUENCE: 1
- PAYLOAD: "token|role|level|score"

### 4.2 UDP Datagram Format

```
TYPE|PAYLOAD
```

**Fields:**
- **TYPE**: Message type code (decimal)
- **PAYLOAD**: Message-specific data
- **No delimiter** (datagram boundary is the frame)

**Example:**
```
1297|12345|cat=animal
```
- TYPE: 1297 (GAME_MOVE)
- PAYLOAD: "sessionId|moveData"

### 4.3 Authentication Messages (TCP with Tokens)

#### Registration

```cpp
REGISTER_REQUEST  = 0x0101  // Decimal: 257
REGISTER_SUCCESS  = 0x0102  // Decimal: 258
REGISTER_FAILED   = 0x0103  // Decimal: 259
```

| Message | Direction | Payload Format | Example |
|---------|-----------|----------------|---------|
| REGISTER_REQUEST | C→S | `username\|password\|role` | `john\|pass123\|0` |
| REGISTER_SUCCESS | S→C | `token\|message` | `eyJhbGc...xyz\|Registration successful` |
| REGISTER_FAILED | S→C | `error_message` | `Username already exists` |

#### Login (with Token Response)

```cpp
LOGIN_REQUEST     = 0x0111  // Decimal: 273
LOGIN_SUCCESS     = 0x0112  // Decimal: 274
LOGIN_FAILED      = 0x0113  // Decimal: 275
```

| Message | Direction | Payload Format | Example |
|---------|-----------|----------------|---------|
| LOGIN_REQUEST | C→S | `username\|password` | `john\|pass123` |
| LOGIN_SUCCESS | S→C | `token\|role\|level\|score` | `eyJhbGciOiJIUzI1NiJ9...xyz\|0\|1\|250` |
| LOGIN_FAILED | S→C | `error_message` | `Invalid credentials` |

**Token Format:**
```
eyJhbGciOiJIUzI1NiJ9.eyJ1c2VyIjoiam9obiIsInJvbGUiOjAsImlhdCI6MTY5ODc2NTQzMiwiZXhwIjoxNjk4NzY5MDMyfQ.signature
│                    │                                                                              │
│    Header          │                    Payload                                                  │  Signature
│   (Base64)         │                   (Base64)                                                  │  (HMAC-SHA256)
```

#### Logout

```cpp
LOGOUT_REQUEST    = 0x0121  // Decimal: 289
LOGOUT_SUCCESS    = 0x0122  // Decimal: 290
```

| Message | Direction | Payload Format | Example |
|---------|-----------|----------------|---------|
| LOGOUT_REQUEST | C→S | `token` | `eyJhbGc...xyz` |
| LOGOUT_SUCCESS | S→C | `message` | `Logged out successfully` |

#### Token Refresh

```cpp
TOKEN_REFRESH_REQUEST  = 0x0131  // Decimal: 305
TOKEN_REFRESH_SUCCESS  = 0x0132  // Decimal: 306
```

| Message | Direction | Payload Format | Example |
|---------|-----------|----------------|---------|
| TOKEN_REFRESH_REQUEST | C→S | `old_token` | `eyJhbGc...xyz` |
| TOKEN_REFRESH_SUCCESS | S→C | `new_token` | `eyJhbGc...abc` |

### 4.4 Study Setup Messages (TCP with Token)

```cpp
SET_LEVEL_REQUEST = 0x0201  // Decimal: 513
SET_LEVEL_SUCCESS = 0x0202  // Decimal: 514
SET_LEVEL_FAILED  = 0x0203  // Decimal: 515
```

| Message | Direction | Payload Format | Example |
|---------|-----------|----------------|---------|
| SET_LEVEL_REQUEST | C→S | `token\|level` | `eyJhbGc...xyz\|1` (0=Beginner, 1=Intermediate, 2=Advanced) |
| SET_LEVEL_SUCCESS | S→C | `message` | `Level set to INTERMEDIATE` |

### 4.5 Content Access Messages (Mixed TCP/UDP)

```cpp
GET_LESSON_LIST_REQUEST    = 0x0301  // Decimal: 769 (TCP)
GET_LESSON_LIST_RESPONSE   = 0x0302  // Decimal: 770 (TCP)

GET_LESSON_CONTENT_REQUEST = 0x0311  // Decimal: 785 (UDP)
LESSON_METADATA_RESPONSE   = 0x0312  // Decimal: 786 (TCP)
CONTENT_CHUNK              = 0x0313  // Decimal: 787 (UDP)
CONTENT_COMPLETE           = 0x0314  // Decimal: 788 (TCP)
```

| Message | Protocol | Direction | Payload Format | Example |
|---------|----------|-----------|----------------|---------|
| GET_LESSON_LIST_REQUEST | **TCP** | C→S | `token` | `eyJhbGc...xyz` |
| GET_LESSON_LIST_RESPONSE | **TCP** | S→C | `lesson1;lesson2;lesson3` | `lesson_b1:Greetings;lesson_b2:Numbers` |
| GET_LESSON_CONTENT_REQUEST | **UDP** | C→S | `token\|lessonId` | `eyJhbGc...xyz\|lesson_b1` |
| LESSON_METADATA_RESPONSE | **TCP** | S→C | `chunks\|size\|type` | `150\|4500000\|video/mp4` |
| CONTENT_CHUNK | **UDP** | S→C | `lessonId\|chunkIndex\|data` | `lesson_b1\|0\|[binary_data]` |
| CONTENT_COMPLETE | **TCP** | S→C | `lessonId\|status` | `lesson_b1\|success` |

### 4.6 Exercise Messages (TCP with Token)

```cpp
SUBMIT_QUIZ_REQUEST     = 0x0401  // Decimal: 1025
SUBMIT_QUIZ_RESPONSE    = 0x0402  // Decimal: 1026

SUBMIT_EXERCISE_REQUEST = 0x0411  // Decimal: 1041
SUBMIT_EXERCISE_RESPONSE= 0x0412  // Decimal: 1042
```

| Message | Direction | Payload Format | Example |
|---------|-----------|----------------|---------|
| SUBMIT_QUIZ_REQUEST | C→S | `token\|quizId\|answers` | `eyJhbGc...xyz\|quiz_1\|A,B,C,A,D` |
| SUBMIT_QUIZ_RESPONSE | S→C | `score\|feedback` | `80\|Good job! 4/5 correct` |
| SUBMIT_EXERCISE_REQUEST | C→S | `token\|exerciseId\|content` | `eyJhbGc...xyz\|ex_1\|My essay...` |
| SUBMIT_EXERCISE_RESPONSE | S→C | `score\|feedback` | `85\|Well written!` |

### 4.7 Game Messages (Mixed TCP/UDP)

```cpp
GAME_START_REQUEST   = 0x0501  // Decimal: 1281 (TCP)
GAME_START_RESPONSE  = 0x0502  // Decimal: 1282 (TCP)

GAME_MOVE_REQUEST    = 0x0511  // Decimal: 1297 (UDP)
GAME_MOVE_RESPONSE   = 0x0512  // Decimal: 1298 (UDP)

GAME_END_REQUEST     = 0x0521  // Decimal: 1313 (TCP)
GAME_FINAL_SCORE     = 0x0522  // Decimal: 1314 (TCP)
```

| Message | Protocol | Direction | Payload Format | Example |
|---------|----------|-----------|----------------|---------|
| GAME_START_REQUEST | **TCP** | C→S | `token\|gameType` | `eyJhbGc...xyz\|Word Matching` |
| GAME_START_RESPONSE | **TCP** | S→C | `sessionId\|items` | `sess_123\|cat=animal;dog=pet` |
| GAME_MOVE_REQUEST | **UDP** | C→S | `sessionId\|moveData` | `sess_123\|cat=animal` |
| GAME_MOVE_RESPONSE | **UDP** | S→C | `result\|score` | `correct\|10` or `incorrect\|0` |
| GAME_END_REQUEST | **TCP** | C→S | `token\|sessionId` | `eyJhbGc...xyz\|sess_123` |
| GAME_FINAL_SCORE | **TCP** | S→C | `totalScore` | `100` |

### 4.8 Voice Chat Messages (File Transfer - Mixed TCP/UDP)

**NEW: Voice Chat is File Transfer!**

```cpp
CHAT_MESSAGE              = 0x0701  // Decimal: 1793 (TCP - text only)
CHAT_MESSAGE_ACK          = 0x0702  // Decimal: 1794 (TCP)

VOICE_FILE_UPLOAD         = 0x0715  // Decimal: 1813 (UDP - upload chunks)
VOICE_MESSAGE_ACK         = 0x0716  // Decimal: 1814 (TCP - confirm receipt)
VOICE_CHAT_NOTIFY         = 0x0717  // Decimal: 1815 (TCP - notify recipient)
VOICE_FILE_REQUEST        = 0x0718  // Decimal: 1816 (UDP - download request)
VOICE_FILE_DATA           = 0x0719  // Decimal: 1817 (UDP - download chunks)
VOICE_FILE_COMPLETE       = 0x071A  // Decimal: 1818 (TCP - transfer complete)
VOICE_FILE_ERROR          = 0x071B  // Decimal: 1819 (TCP - transfer error)
```

| Message | Protocol | Direction | Payload Format | Example |
|---------|----------|-----------|----------------|---------|
| CHAT_MESSAGE | **TCP** | C→S or S→C | `token\|recipient\|message` | `eyJhbGc...xyz\|bob\|Hello!` |
| CHAT_MESSAGE_ACK | **TCP** | S→C | `delivered` | `Message delivered` |
| VOICE_FILE_UPLOAD | **UDP** | C→S | `token\|fileId\|chunkIndex\|totalChunks\|data` | `eyJ...\|12345\|0\|15\|[audio_data]` |
| VOICE_MESSAGE_ACK | **TCP** | S→C | `fileId\|size\|status` | `12345\|245000\|received` |
| VOICE_CHAT_NOTIFY | **TCP** | S→C | `sender\|fileId\|filename\|size` | `alice\|12345\|voice.wav\|245000` |
| VOICE_FILE_REQUEST | **UDP** | C→S | `token\|fileId` | `eyJhbGc...xyz\|12345` |
| VOICE_FILE_DATA | **UDP** | S→C | `fileId\|chunkIndex\|totalChunks\|data` | `12345\|0\|15\|[audio_data]` |
| VOICE_FILE_COMPLETE | **TCP** | C→S | `fileId\|status` | `12345\|success` |
| VOICE_FILE_ERROR | **TCP** | S→C | `fileId\|error` | `12345\|checksum_failed` |

**Voice File Upload Process:**
```
1. Client records audio → save as temp.wav
2. Split file into chunks (1KB each)
3. Send VOICE_FILE_UPLOAD chunks via UDP
4. Server reassembles and stores
5. Server sends VOICE_MESSAGE_ACK via TCP
6. Server notifies recipient via VOICE_CHAT_NOTIFY (TCP)
7. Recipient downloads via VOICE_FILE_REQUEST/DATA (UDP)
8. Recipient confirms with VOICE_FILE_COMPLETE (TCP)
```

### 4.9 Assessment Messages (TCP with Token)

```cpp
GET_SCORE_REQUEST    = 0x0601  // Decimal: 1537
GET_SCORE_RESPONSE   = 0x0602  // Decimal: 1538

GET_FEEDBACK_REQUEST = 0x0611  // Decimal: 1553
GET_FEEDBACK_RESPONSE= 0x0612  // Decimal: 1554

SEND_FEEDBACK_REQUEST= 0x0621  // Decimal: 1569 (Teacher only)
SEND_FEEDBACK_SUCCESS= 0x0622  // Decimal: 1570
```

| Message | Direction | Payload Format | Example |
|---------|-----------|----------------|---------|
| GET_SCORE_REQUEST | C→S | `token` | `eyJhbGc...xyz` |
| GET_SCORE_RESPONSE | S→C | `score` | `250` |
| GET_FEEDBACK_REQUEST | C→S | `token` | `eyJhbGc...xyz` |
| GET_FEEDBACK_RESPONSE | S→C | `feedback1;feedback2;...` | `Good work;Keep practicing` |
| SEND_FEEDBACK_REQUEST | C→S | `token\|student\|exerciseId\|feedback` | `eyJ...\|john\|ex_1\|Good!` |
| SEND_FEEDBACK_SUCCESS | S→C | `message` | `Feedback sent` |

### 4.10 Admin Messages (TCP with Token)

```cpp
ADD_GAME_ITEM_REQUEST = 0x0801  // Decimal: 2049
ADD_GAME_ITEM_SUCCESS = 0x0802  // Decimal: 2050
ADD_GAME_ITEM_FAILED  = 0x0803  // Decimal: 2051
```

| Message | Direction | Payload Format | Example |
|---------|-----------|----------------|---------|
| ADD_GAME_ITEM_REQUEST | C→S | `token\|gameType\|item` | `eyJ...\|Word Matching\|cat=animal` |
| ADD_GAME_ITEM_SUCCESS | S→C | `message` | `Item added` |

### 4.11 System Messages (TCP)

```cpp
HEARTBEAT_REQUEST      = 0x0901  // Decimal: 2305
HEARTBEAT_RESPONSE     = 0x0902  // Decimal: 2306

ERROR_MESSAGE          = 0x0911  // Decimal: 2321

DISCONNECT_NOTIFICATION= 0x0921  // Decimal: 2337
```

| Message | Direction | Payload Format | Example |
|---------|-----------|----------------|---------|
| HEARTBEAT_REQUEST | C→S | `token` | `eyJhbGc...xyz` |
| HEARTBEAT_RESPONSE | S→C | `ok` | `alive` |
| ERROR_MESSAGE | S→C | `errorCode\|description` | `5\|Token expired` |

### 4.12 Error Codes

| Code | Name | Description |
|------|------|-------------|
| 0 | SUCCESS | Operation successful |
| 1 | INVALID_FORMAT | Message format invalid |
| 2 | INVALID_CREDENTIALS | Login failed |
| 3 | USER_ALREADY_EXISTS | Registration failed |
| 4 | USER_NOT_FOUND | User doesn't exist |
| 5 | TOKEN_EXPIRED | Token expired, re-login |
| 6 | TOKEN_INVALID | Token signature invalid |
| 7 | NOT_AUTHENTICATED | Token missing/invalid |
| 8 | PERMISSION_DENIED | Insufficient privileges |
| 9 | RESOURCE_NOT_FOUND | Resource not found |
| 10 | INTERNAL_ERROR | Server error |

---

## Summary

This document describes the **actual implementation** of the English Learning Platform:

### ✅ Key Features

1. **Hybrid TCP/UDP Protocol**
   - TCP for reliable control (auth, chat, session)
   - UDP for fast data (games, content, files)

2. **Token-Based Authentication**
   - JWT tokens issued on login
   - Tokens in all authenticated requests
   - No password re-transmission

3. **Voice Chat as File Transfer**
   - Record audio files
   - Upload via UDP chunks
   - Download via UDP chunks
   - Notifications via TCP

4. **UDP for Games & Content**
   - Low latency game moves
   - Fast content streaming
   - Custom reliability layer

5. **Complete Message Protocol**
   - 40+ message types
   - Clear wire formats
   - TCP and UDP variants

---

**Document Version**: 1.0  
**Last Updated**: October 27, 2025  
**Status**: Final Technical Specification  
**Architecture**: Hybrid TCP/UDP with Token Authentication

