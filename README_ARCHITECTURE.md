# ⚠️ IMPORTANT: Architecture Overview

## Quick Architecture Summary

This document provides a quick overview of the **actual implementation architecture**.

---

## 🔄 Hybrid TCP/UDP Protocol

The system uses **BOTH** TCP and UDP:

### TCP (Port 8080) - Reliable Control
- ✅ Authentication & token management
- ✅ Text chat messages
- ✅ Session management & heartbeat
- ✅ Administrative commands
- ✅ Error messages

### UDP (Port 8081) - Fast Data
- ✅ Game moves & updates
- ✅ Lesson content streaming
- ✅ Voice file transfer
- ✅ Video/audio content delivery

---

## 🔐 Token-Based Authentication

**NOT password re-transmission!**

```
Login Flow:
1. Client sends: username|password (TCP)
2. Server responds: JWT_TOKEN|role|level|score (TCP)
3. Client stores token locally
4. All subsequent requests: TOKEN|request_data
5. Server validates token (no DB lookup needed)
```

**Benefits:**
- More secure
- Tokens expire
- Can be revoked
- No password re-transmission

---

## 🎤 Voice Messages = File Transfer

**NOT real-time voice calls!**

```
Voice Message Flow:
1. Client records audio → saves as .wav file
2. Client uploads file via UDP (chunked)
3. Server stores file in /data/voice/
4. Server notifies recipient via TCP
5. Recipient downloads file via UDP
6. Recipient plays audio file
```

**Why:**
- Simpler implementation
- Works offline (async)
- No complex real-time streaming
- Reliable delivery with custom UDP protocol

---

## 🎮 Games Use UDP

**For speed and low latency!**

```
Game Flow:
1. GAME_START (TCP) → reliable setup
2. GAME_MOVE (UDP) → fast moves
3. MOVE_RESULT (UDP) → fast response
4. ... many fast UDP exchanges ...
5. GAME_END (TCP) → reliable completion
```

**Why UDP:**
- Lower latency
- Speed > reliability for games
- Can tolerate packet loss
- Better gaming experience

---

## 📚 Content Uses UDP

**Lesson videos/audio streamed via UDP**

```
Content Flow:
1. GET_LESSON_LIST (TCP) → reliable list
2. GET_LESSON_CONTENT (UDP) → fast metadata
3. CONTENT_CHUNK (UDP) → stream data
4. ... multiple fast chunks ...
5. CONTENT_COMPLETE (TCP) → confirm
```

**Why UDP:**
- Faster streaming
- Can tolerate minor loss
- No head-of-line blocking
- Better video performance

---

## 📋 Protocol Summary Table

| Feature | Protocol | Port | Why |
|---------|----------|------|-----|
| Login/Register | TCP | 8080 | Must be reliable |
| Text Chat | TCP | 8080 | Order important |
| Session/Heartbeat | TCP | 8080 | Reliability critical |
| Voice Files | UDP | 8081 | Fast transfer |
| Game Moves | UDP | 8081 | Low latency |
| Lesson Content | UDP | 8081 | Streaming speed |
| Game Start/End | TCP | 8080 | Reliability critical |

---

## 📁 Key Message Updates

### Authentication (with Tokens)

| Message | Protocol | Payload |
|---------|----------|---------|
| LOGIN_REQUEST | TCP | `username\|password` |
| LOGIN_SUCCESS | TCP | `**token**\|role\|level\|score` |
| Other requests | TCP/UDP | `**token**\|data` |

### Voice Messages (File Transfer)

| Message | Protocol | Purpose |
|---------|----------|---------|
| VOICE_FILE_UPLOAD | **UDP** | Upload chunks |
| VOICE_MESSAGE_NOTIFY | **TCP** | Notify recipient |
| VOICE_FILE_REQUEST | **UDP** | Download request |
| VOICE_FILE_DATA | **UDP** | Download chunks |

### Games

| Message | Protocol | Purpose |
|---------|----------|---------|
| GAME_START | **TCP** | Setup game |
| GAME_MOVE | **UDP** | Fast moves |
| GAME_MOVE_RESULT | **UDP** | Fast response |
| GAME_END | **TCP** | Finish game |

### Content

| Message | Protocol | Purpose |
|---------|----------|---------|
| GET_LESSON_LIST | **TCP** | Get list |
| GET_LESSON_CONTENT | **UDP** | Request content |
| CONTENT_CHUNK | **UDP** | Stream data |
| CONTENT_COMPLETE | **TCP** | Confirm |

---

## 🔍 For More Details

- **Complete Architecture**: See [ARCHITECTURE_CORRECTIONS.md](ARCHITECTURE_CORRECTIONS.md)
- **Technical Docs**: See [TECHNICAL_DOCUMENTATION.md](TECHNICAL_DOCUMENTATION.md)
- **Visual Diagrams**: See [ARCHITECTURE_DIAGRAMS.md](ARCHITECTURE_DIAGRAMS.md)
- **Quick Reference**: See [QUICK_REFERENCE.md](QUICK_REFERENCE.md)

---

## ✅ Key Takeaways

1. **Hybrid System**: TCP + UDP (not just TCP!)
2. **Token Auth**: JWT-like tokens (not password re-send)
3. **Voice = Files**: Upload/download voice files (not real-time calls)
4. **UDP for Speed**: Games and content use UDP for performance
5. **TCP for Reliability**: Auth, chat, control use TCP for safety

---

**Last Updated**: October 27, 2025  
**Status**: Accurate Architecture Description  
**Read This First**: Before diving into detailed documentation!

