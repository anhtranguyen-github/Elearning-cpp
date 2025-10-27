# English Learning Platform - Architecture Corrections & Clarifications

## Important Architectural Updates

This document clarifies the actual architecture implementation with corrections to the initial documentation.

---

## 1. Hybrid TCP/UDP Architecture

### 1.1 Protocol Split

The system uses **BOTH TCP and UDP** for different purposes:

```
┌────────────────────────────────────────────────────────────────────┐
│                   HYBRID PROTOCOL ARCHITECTURE                      │
└────────────────────────────────────────────────────────────────────┘

        Client                                Server
          │                                     │
          │                                     │
    ┌─────▼─────┐                         ┌─────▼─────┐
    │    TCP    │◄═══════════════════════►│    TCP    │
    │ Port 8080 │  Reliable Connection    │ Port 8080 │
    └───────────┘                         └───────────┘
         │                                      │
         │ • Authentication & Tokens            │
         │ • Text Chat Messages                 │
         │ • Session Management                 │
         │ • Heartbeat                          │
         │ • Administrative Commands            │
         │ • Error Messages                     │
         │                                      │
    ┌─────▼─────┐                         ┌─────▼─────┐
    │    UDP    │◄═══════════════════════►│    UDP    │
    │ Port 8081 │  Fast Transmission      │ Port 8081 │
    └───────────┘                         └───────────┘
         │                                      │
         │ • Game Data & Moves                  │
         │ • Lesson Content Streaming           │
         │ • Voice File Transfer                │
         │ • Video/Audio Content                │
         │ • Real-time Updates                  │
         │                                      │
```

### 1.2 Why Hybrid TCP/UDP?

| Protocol | Use Case | Reason |
|----------|----------|---------|
| **TCP** | Authentication, Chat, Commands | Requires reliability, order guarantee |
| **TCP** | Session Management, Heartbeat | Must be reliable for state management |
| **UDP** | Games | Speed more important than reliability |
| **UDP** | Content Streaming (lessons) | Can tolerate packet loss, faster |
| **UDP** | File Transfer (voice files) | Chunked transfer with custom reliability |

---

## 2. Token-Based Authentication (Not Password Re-transmission)

### 2.1 Authentication Flow with Tokens

```
Client                          Server                      Database
  │                               │                            │
  │  1. LOGIN_REQUEST             │                            │
  │  (username|password)          │                            │
  │ ─────────────────────────────►│                            │
  │                               │  Authenticate              │
  │                               │ ──────────────────────────►│
  │                               │  Verify credentials        │
  │                               │ ◄──────────────────────────│
  │                               │                            │
  │                               │  Generate JWT Token        │
  │                               │  token = JWT.create({      │
  │                               │    user: username,         │
  │                               │    role: role,             │
  │                               │    exp: timestamp + 3600   │
  │                               │  })                        │
  │                               │                            │
  │  2. LOGIN_SUCCESS             │                            │
  │  (token|role|level|score)     │                            │
  │ ◄─────────────────────────────│                            │
  │                               │                            │
  │  Store token locally          │                            │
  │                               │                            │
  │  3. Subsequent Requests       │                            │
  │  (token|request_data)         │                            │
  │ ─────────────────────────────►│                            │
  │                               │  Verify token              │
  │                               │  Extract user from token   │
  │                               │  Check expiration          │
  │                               │                            │
  │  4. Response                  │                            │
  │ ◄─────────────────────────────│                            │
```

### 2.2 Token Structure

```cpp
struct Token {
    std::string userId;
    std::string username;
    UserRole role;
    uint64_t issuedAt;
    uint64_t expiresAt;
    std::string signature;  // HMAC-SHA256
};

// Token Format (Base64 encoded):
// header.payload.signature
// Example:
// eyJhbGc6IkhTMjU2In0.eyJ1c2VyIjoiam9obiIsInJvbGUiOjAsImV4cCI6MTY5ODc2NTQzMn0.abc123...
```

### 2.3 Updated Authentication Messages

| Message | Direction | Payload Format | Example |
|---------|-----------|----------------|---------|
| LOGIN_REQUEST | C→S | `username\|password` | `john\|pass123` |
| LOGIN_SUCCESS | S→C | `token\|role\|level\|score` | `eyJhbGc...abc123\|0\|1\|250` |
| *All Other Requests* | C→S | `token\|data` | `eyJhbGc...abc123\|quiz_1\|A,B,C` |

**Benefits:**
- ✅ No password re-transmission after login
- ✅ Tokens can be invalidated server-side
- ✅ Tokens expire automatically
- ✅ Stateless validation (server can verify without database lookup)

---

## 3. Voice Messages (File Transfer, Not Real-Time Calls)

### 3.1 Voice Message Architecture

**Important:** Voice "call" is actually **voice file transfer** in chat!

```
┌────────────────────────────────────────────────────────────────────┐
│                    VOICE MESSAGE WORKFLOW                           │
└────────────────────────────────────────────────────────────────────┘

Client A (Sender)                                         Client B (Receiver)
     │                                                           │
     │ 1. Record voice (local audio file)                       │
     │    file: voice_20251027_103045.wav                       │
     │                                                           │
     │ 2. Send file via UDP                                     │
     │ ──────────────► Server ──────────────►                   │
     │    (Chunked packets)    Store file    Notify via TCP     │
     │                         in /data/     CHAT_MESSAGE        │
     │                         voice/        with file_id        │
     │                                                           │
     │ 3. TCP confirmation                                       │
     │ ◄─────────────── Server                                  │
     │    VOICE_MESSAGE_ACK                                      │
     │    (file_id, size)                                        │
     │                                                           │
     │                                       4. Download request │
     │                                       Request file_id     │
     │                         Server ◄──────────────────────    │
     │                                                           │
     │                                       5. File transfer    │
     │                         Server ──────────────────────►    │
     │                         (UDP chunks)                      │
     │                                                           │
     │                                       6. Play audio       │
     │                                       Decode & play       │
```

### 3.2 Voice Message Protocol

#### Step 1: Send Voice File (UDP)

```cpp
// Client sends voice file in chunks
struct VoiceFileChunk {
    uint32_t fileId;        // Unique file identifier
    uint32_t chunkIndex;    // Chunk number (0, 1, 2...)
    uint32_t totalChunks;   // Total number of chunks
    uint16_t chunkSize;     // Size of this chunk
    char data[1024];        // Audio data (max 1KB per chunk)
};

// UDP Message Format:
// VOICE_FILE|fileId|chunkIndex|totalChunks|chunkSize|data
```

#### Step 2: Server Stores File

```cpp
// Server reassembles chunks
void Server::handleVoiceFileChunk(const VoiceFileChunk& chunk) {
    // Store chunk
    voiceFiles_[chunk.fileId].chunks[chunk.chunkIndex] = chunk.data;
    
    // Check if all chunks received
    if (allChunksReceived(chunk.fileId)) {
        // Reassemble file
        std::string filename = "data/voice/" + std::to_string(chunk.fileId) + ".wav";
        saveVoiceFile(chunk.fileId, filename);
        
        // Notify recipient via TCP
        sendChatNotification(recipient, sender, chunk.fileId, filename);
    }
}
```

#### Step 3: Recipient Downloads (UDP)

```cpp
// Recipient requests file
// UDP: GET_VOICE_FILE|fileId
// Server responds with chunks
```

### 3.3 Updated Communication Messages

| Message | Protocol | Direction | Payload Format | Purpose |
|---------|----------|-----------|----------------|---------|
| CHAT_MESSAGE | TCP | C→S→C | `recipient\|message` | Text chat |
| VOICE_FILE_UPLOAD | UDP | C→S | `fileId\|chunk\|data` | Upload voice |
| VOICE_MESSAGE_NOTIFY | TCP | S→C | `sender\|fileId\|filename` | Notify of voice |
| VOICE_FILE_DOWNLOAD | UDP | C→S | `fileId` | Request voice |
| VOICE_FILE_DATA | UDP | S→C | `fileId\|chunk\|data` | Send voice |

---

## 4. UDP for Games

### 4.1 Why UDP for Games?

Games require:
- ✅ **Speed** over reliability
- ✅ **Real-time** updates
- ✅ Tolerance for packet loss (game can interpolate)
- ✅ Lower latency

### 4.2 Game Communication Flow

```
Client                          Server (UDP Port 8081)
  │                                     │
  │ 1. GAME_START_REQUEST (TCP)        │
  │ ───────────────────────────────►   │
  │    token|Word Matching              │
  │                                     │
  │ 2. GAME_START_RESPONSE (TCP)       │
  │ ◄───────────────────────────────   │
  │    sessionId|items                  │
  │                                     │
  │ 3. GAME_MOVE (UDP)                 │
  │ ═══════════════════════════════►   │
  │    sessionId|cat=animal             │
  │                                     │
  │ 4. GAME_MOVE_RESULT (UDP)          │
  │ ◄═══════════════════════════════   │
  │    correct|score:10                 │
  │                                     │
  │ 5. GAME_MOVE (UDP)                 │
  │ ═══════════════════════════════►   │
  │                                     │
  │ 6. GAME_MOVE_RESULT (UDP)          │
  │ ◄═══════════════════════════════   │
  │                                     │
  │ 7. GAME_END (TCP)                  │
  │ ───────────────────────────────►   │
  │                                     │
  │ 8. FINAL_SCORE (TCP)               │
  │ ◄───────────────────────────────   │
```

**Note:**
- Game START/END use **TCP** (need reliability)
- Game MOVES use **UDP** (need speed)

### 4.3 UDP Game Message Format

```cpp
// UDP Datagram for game moves
struct GameMovePacket {
    uint32_t sessionId;     // Game session identifier
    uint16_t sequenceNum;   // For ordering (optional)
    char moveData[256];     // Move data (e.g., "cat=animal")
};

// Wire format (UDP):
// GAME_MOVE|sessionId|sequenceNum|moveData
// Example: GAME_MOVE|12345|1|cat=animal
```

---

## 5. UDP for Content Access (Lessons)

### 5.1 Why UDP for Lesson Content?

Lesson content (videos, audio) benefits from UDP because:
- ✅ Streaming performance
- ✅ Can tolerate minor packet loss
- ✅ Faster than TCP for large files
- ✅ No head-of-line blocking

### 5.2 Lesson Content Streaming

```
Client                          Server (UDP Port 8081)
  │                                     │
  │ 1. GET_LESSON_LIST (TCP)           │
  │ ───────────────────────────────►   │
  │    token                            │
  │                                     │
  │ 2. LESSON_LIST (TCP)               │
  │ ◄───────────────────────────────   │
  │    lesson_b1:Greetings;...          │
  │                                     │
  │ 3. GET_LESSON_CONTENT (UDP)        │
  │ ═══════════════════════════════►   │
  │    token|lesson_b1                  │
  │                                     │
  │ 4. LESSON_METADATA (TCP)           │
  │ ◄───────────────────────────────   │
  │    video_url|audio_url|text|chunks  │
  │                                     │
  │ 5. STREAM_VIDEO_CHUNK (UDP)        │
  │ ◄═══════════════════════════════   │
  │    chunk_0 (video data)             │
  │                                     │
  │ 6. STREAM_VIDEO_CHUNK (UDP)        │
  │ ◄═══════════════════════════════   │
  │    chunk_1 (video data)             │
  │                                     │
  │ ... (multiple chunks)               │
  │                                     │
  │ 7. STREAM_COMPLETE (TCP)           │
  │ ◄───────────────────────────────   │
  │    transfer_complete                │
```

### 5.3 UDP Content Packet Format

```cpp
// UDP packet for content streaming
struct ContentChunk {
    uint32_t contentId;     // Lesson/file identifier
    uint32_t chunkIndex;    // Chunk number
    uint32_t totalChunks;   // Total chunks in file
    uint16_t chunkSize;     // Size of this chunk
    char data[4096];        // Content data (up to 4KB)
    uint16_t checksum;      // CRC16 for error detection
};

// Wire format:
// CONTENT_CHUNK|contentId|chunkIndex|totalChunks|chunkSize|data|checksum
```

---

## 6. File Transfer Protocol (Generic)

### 6.1 File Transfer Over UDP

```cpp
// Generic file transfer protocol used for:
// - Voice messages
// - Lesson videos/audio
// - Any user-uploaded content

class FileTransfer {
public:
    struct FileMetadata {
        uint32_t fileId;
        std::string filename;
        uint32_t fileSize;
        uint32_t chunkCount;
        std::string contentType;  // "audio/wav", "video/mp4", etc.
        std::string hash;         // SHA256 for verification
    };
    
    struct FileChunk {
        uint32_t fileId;
        uint32_t chunkIndex;
        uint32_t chunkSize;
        char data[4096];
        uint16_t checksum;
    };
    
    // Methods
    void sendFile(const std::string& filepath, uint32_t fileId);
    void receiveChunk(const FileChunk& chunk);
    bool isComplete(uint32_t fileId);
    void saveFile(uint32_t fileId, const std::string& outputPath);
};
```

### 6.2 Reliability Layer for UDP

Since UDP is unreliable, we add custom reliability:

```cpp
class UDPReliability {
public:
    // Send chunk with acknowledgment
    bool sendChunkWithAck(const FileChunk& chunk, int timeout_ms = 1000) {
        int retries = 0;
        while (retries < 3) {
            sendUDP(chunk);
            
            // Wait for ACK (TCP)
            if (waitForAck(chunk.fileId, chunk.chunkIndex, timeout_ms)) {
                return true;
            }
            retries++;
        }
        return false;  // Failed after 3 retries
    }
    
    // Request missing chunks
    void requestMissingChunks(uint32_t fileId, const std::vector<uint32_t>& missingChunks) {
        for (uint32_t chunkIndex : missingChunks) {
            sendUDP(FileChunkRequest{fileId, chunkIndex});
        }
    }
};
```

---

## 7. Updated Message Type Codes

### 7.1 Authentication with Tokens

| Code | Type | Protocol | Payload |
|------|------|----------|---------|
| 257 | REGISTER_REQUEST | TCP | `username\|password\|role` |
| 258 | REGISTER_SUCCESS | TCP | `token\|message` |
| 273 | LOGIN_REQUEST | TCP | `username\|password` |
| 274 | LOGIN_SUCCESS | TCP | `token\|role\|level\|score` |
| 289 | LOGOUT_REQUEST | TCP | `token` |
| 290 | LOGOUT_SUCCESS | TCP | `Logged out` |
| 2341 | TOKEN_REFRESH | TCP | `old_token` |
| 2342 | TOKEN_REFRESHED | TCP | `new_token` |

### 7.2 Voice Messages (File Transfer)

| Code | Type | Protocol | Payload |
|------|------|----------|---------|
| 1793 | CHAT_MESSAGE | TCP | `token\|recipient\|message` |
| 1815 | VOICE_FILE_UPLOAD | UDP | `fileId\|chunkIndex\|data` |
| 1816 | VOICE_MESSAGE_NOTIFY | TCP | `sender\|fileId\|filename\|size` |
| 1817 | VOICE_FILE_REQUEST | UDP | `token\|fileId` |
| 1818 | VOICE_FILE_DATA | UDP | `fileId\|chunkIndex\|data` |
| 1819 | VOICE_FILE_COMPLETE | TCP | `fileId\|success` |

### 7.3 Games (UDP)

| Code | Type | Protocol | Payload |
|------|------|----------|---------|
| 1281 | GAME_START_REQUEST | TCP | `token\|gameType` |
| 1282 | GAME_START_RESPONSE | TCP | `sessionId\|items` |
| 1297 | GAME_MOVE_REQUEST | **UDP** | `sessionId\|moveData` |
| 1298 | GAME_MOVE_RESPONSE | **UDP** | `result\|score` |
| 1313 | GAME_END | TCP | `sessionId` |
| 1314 | GAME_FINAL_SCORE | TCP | `score` |

### 7.4 Content Access (UDP)

| Code | Type | Protocol | Payload |
|------|------|----------|---------|
| 769 | GET_LESSON_LIST | TCP | `token` |
| 770 | LESSON_LIST_RESPONSE | TCP | `lesson1;lesson2;...` |
| 785 | GET_LESSON_CONTENT | **UDP** | `token\|lessonId` |
| 786 | LESSON_METADATA | TCP | `video_url\|audio_url\|text\|chunks` |
| 787 | CONTENT_CHUNK | **UDP** | `contentId\|chunkIndex\|data` |
| 788 | CONTENT_COMPLETE | TCP | `contentId\|success` |

---

## 8. Updated Communication Diagrams

### 8.1 Voice Message Communication

```
Client A                 Server (TCP+UDP)              Client B
  │                             │                         │
  │ 1. Record voice             │                         │
  │    (local file)             │                         │
  │                             │                         │
  │ 2. VOICE_FILE_UPLOAD (UDP)  │                         │
  │ ═══════════════════════════►│                         │
  │    (multiple chunks)        │ Store file              │
  │                             │                         │
  │ 3. VOICE_MESSAGE_ACK (TCP)  │                         │
  │ ◄───────────────────────────│                         │
  │    fileId|size              │                         │
  │                             │                         │
  │                             │ 4. VOICE_MESSAGE_NOTIFY │
  │                             │    (TCP)                │
  │                             │ ───────────────────────►│
  │                             │    sender|fileId        │
  │                             │                         │
  │                             │ 5. VOICE_FILE_REQUEST   │
  │                             │    (UDP)                │
  │                             │ ◄═══════════════════════│
  │                             │    fileId               │
  │                             │                         │
  │                             │ 6. VOICE_FILE_DATA      │
  │                             │    (UDP chunks)         │
  │                             │ ═══════════════════════►│
  │                             │                         │
  │                             │ 7. PLAY audio           │
  │                             │                         │
```

### 8.2 Game with UDP

```
Client                Server (TCP:8080, UDP:8081)
  │                             │
  │ GAME_START (TCP)            │
  │ ───────────────────────────►│
  │                             │
  │ GAME_SESSION (TCP)          │
  │ ◄───────────────────────────│
  │                             │
  │ GAME_MOVE #1 (UDP)          │
  │ ═══════════════════════════►│
  │                             │ Process
  │ MOVE_RESULT #1 (UDP)        │
  │ ◄═══════════════════════════│
  │                             │
  │ GAME_MOVE #2 (UDP)          │
  │ ═══════════════════════════►│
  │                             │
  │ MOVE_RESULT #2 (UDP)        │
  │ ◄═══════════════════════════│
  │                             │
  │ ...  (fast UDP exchange)    │
  │                             │
  │ GAME_END (TCP)              │
  │ ───────────────────────────►│
  │                             │
  │ FINAL_SCORE (TCP)           │
  │ ◄───────────────────────────│
```

---

## 9. Implementation Notes

### 9.1 Socket Management

```cpp
class HybridClient {
private:
    SOCKET tcpSocket_;     // Port 8080 - reliable control
    SOCKET udpSocket_;     // Port 8081 - fast data
    std::string authToken_;  // Stored after login
    
public:
    // TCP operations
    bool sendTCP(const Message& msg);
    Message receiveTCP();
    
    // UDP operations
    bool sendUDP(const void* data, size_t len);
    size_t receiveUDP(void* buffer, size_t maxLen);
    
    // High-level operations
    bool sendVoiceFile(const std::string& filepath, const std::string& recipient);
    bool sendGameMove(uint32_t sessionId, const std::string& move);
    bool requestLessonContent(const std::string& lessonId);
};
```

### 9.2 Server Socket Handling

```cpp
class HybridServer {
private:
    SOCKET tcpListenSocket_;   // Port 8080
    SOCKET udpSocket_;         // Port 8081
    
    std::map<SOCKET, ClientHandler*> tcpClients_;
    std::map<std::string, UDPSession*> udpSessions_;  // token -> session
    
public:
    void run() {
        while (running_) {
            // Poll both TCP and UDP
            pollfd fds[2];
            fds[0].fd = tcpListenSocket_;
            fds[0].events = POLLIN;
            fds[1].fd = udpSocket_;
            fds[1].events = POLLIN;
            
            poll(fds, 2, 1000);
            
            if (fds[0].revents & POLLIN) {
                handleTCPEvent();
            }
            if (fds[1].revents & POLLIN) {
                handleUDPEvent();
            }
        }
    }
};
```

---

## 10. Summary of Changes

### ✅ Key Architectural Corrections

1. **Hybrid Protocol**: TCP + UDP, not just TCP
2. **Token Authentication**: JWT-like tokens, not password re-transmission
3. **Voice Messages**: File transfer (UDP), not real-time calls
4. **Games**: UDP for moves, TCP for start/end
5. **Content**: UDP streaming for lessons, TCP for metadata
6. **File Transfer**: Custom UDP protocol with reliability layer

### ✅ Benefits of This Architecture

| Feature | Benefit |
|---------|---------|
| Hybrid TCP/UDP | Best of both worlds - reliability + speed |
| Token Auth | More secure, no password re-transmission |
| UDP for games | Lower latency, better gaming experience |
| UDP for content | Faster streaming, can tolerate loss |
| File transfer | Efficient voice message delivery |

---

**Document Version**: 1.0  
**Last Updated**: October 27, 2025  
**Status**: Architectural Corrections Applied  
**Related Docs**: TECHNICAL_DOCUMENTATION.md, ARCHITECTURE_DIAGRAMS.md

