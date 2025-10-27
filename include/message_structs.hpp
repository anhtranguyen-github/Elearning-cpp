#ifndef MESSAGE_STRUCTS_HPP
#define MESSAGE_STRUCTS_HPP

#include "common.hpp"

// Message type codes following protocol design principles
enum class MessageType : uint16_t {
    // Authentication and account management (0x01xx)
    REGISTER_REQUEST = 0x0101,
    REGISTER_SUCCESS = 0x0102,
    REGISTER_FAILED = 0x0103,
    
    LOGIN_REQUEST = 0x0111,
    LOGIN_SUCCESS = 0x0112,
    LOGIN_FAILED = 0x0113,
    
    LOGOUT_REQUEST = 0x0121,
    LOGOUT_SUCCESS = 0x0122,
    
    // Study setup (0x02xx)
    SET_LEVEL_REQUEST = 0x0201,
    SET_LEVEL_SUCCESS = 0x0202,
    SET_LEVEL_FAILED = 0x0203,
    
    // Content access (0x03xx)
    GET_LESSON_LIST_REQUEST = 0x0301,
    GET_LESSON_LIST_RESPONSE = 0x0302,
    
    GET_LESSON_CONTENT_REQUEST = 0x0311,
    GET_LESSON_CONTENT_RESPONSE = 0x0312,
    
    // Exercises and tests (0x04xx)
    SUBMIT_QUIZ_REQUEST = 0x0401,
    SUBMIT_QUIZ_RESPONSE = 0x0402,
    
    SUBMIT_EXERCISE_REQUEST = 0x0411,
    SUBMIT_EXERCISE_RESPONSE = 0x0412,
    
    // Games (0x05xx)
    GAME_START_REQUEST = 0x0501,
    GAME_START_RESPONSE = 0x0502,
    
    GAME_MOVE_REQUEST = 0x0511,
    GAME_MOVE_RESPONSE = 0x0512,
    
    GAME_END_NOTIFICATION = 0x0521,
    
    // Feedback and assessment (0x06xx)
    GET_SCORE_REQUEST = 0x0601,
    GET_SCORE_RESPONSE = 0x0602,
    
    GET_FEEDBACK_REQUEST = 0x0611,
    GET_FEEDBACK_RESPONSE = 0x0612,
    
    SEND_FEEDBACK_REQUEST = 0x0621, // Teacher sends feedback
    SEND_FEEDBACK_SUCCESS = 0x0622,
    
    // Communication (0x07xx)
    CHAT_MESSAGE = 0x0701,
    CHAT_MESSAGE_ACK = 0x0702,
    
    VOICE_CALL_REQUEST = 0x0711,
    VOICE_CALL_ACCEPT = 0x0712,
    VOICE_CALL_REJECT = 0x0713,
    VOICE_CALL_END = 0x0714,
    
    // Admin operations (0x08xx)
    ADD_GAME_ITEM_REQUEST = 0x0801,
    ADD_GAME_ITEM_SUCCESS = 0x0802,
    ADD_GAME_ITEM_FAILED = 0x0803,
    
    // System messages (0x09xx)
    HEARTBEAT_REQUEST = 0x0901,
    HEARTBEAT_RESPONSE = 0x0902,
    
    ERROR_MESSAGE = 0x0911,
    
    DISCONNECT_NOTIFICATION = 0x0921,
    
    // Unknown/invalid message
    UNKNOWN = 0xFFFF
};

// Error codes
enum class ErrorCode : uint16_t {
    SUCCESS = 0,
    INVALID_FORMAT = 1,
    INVALID_CREDENTIALS = 2,
    USER_ALREADY_EXISTS = 3,
    USER_NOT_FOUND = 4,
    NOT_AUTHENTICATED = 5,
    PERMISSION_DENIED = 6,
    RESOURCE_NOT_FOUND = 7,
    INTERNAL_ERROR = 8,
    DATABASE_ERROR = 9,
    INVALID_PARAMETER = 10
};

// Message header structure (fixed size for efficient parsing)
struct MessageHeader {
    MessageType type;
    uint16_t payloadLength;
    uint32_t sequenceNumber;
    
    MessageHeader() : type(MessageType::UNKNOWN), payloadLength(0), sequenceNumber(0) {}
    MessageHeader(MessageType t, uint16_t len, uint32_t seq = 0) 
        : type(t), payloadLength(len), sequenceNumber(seq) {}
};

// Base message structure
struct Message {
    MessageHeader header;
    std::string payload;
    
    Message() = default;
    Message(MessageType type, const std::string& data = "") 
        : header(type, static_cast<uint16_t>(data.length())), payload(data) {}
    
    // Serialize message to wire format: TYPE|LENGTH|SEQUENCE|PAYLOAD\n
    std::string serialize() const {
        std::ostringstream oss;
        oss << static_cast<uint16_t>(header.type) << "|"
            << header.payloadLength << "|"
            << header.sequenceNumber << "|"
            << payload << "\n";
        return oss.str();
    }
    
    // Deserialize message from wire format
    static Message deserialize(const std::string& data) {
        Message msg;
        
        // Find first three delimiters to split header from payload
        size_t pos1 = data.find('|');
        if (pos1 == std::string::npos) {
            msg.header.type = MessageType::UNKNOWN;
            return msg;
        }
        
        size_t pos2 = data.find('|', pos1 + 1);
        if (pos2 == std::string::npos) {
            msg.header.type = MessageType::UNKNOWN;
            return msg;
        }
        
        size_t pos3 = data.find('|', pos2 + 1);
        if (pos3 == std::string::npos) {
            msg.header.type = MessageType::UNKNOWN;
            return msg;
        }
        
        try {
            // Parse header fields
            msg.header.type = static_cast<MessageType>(std::stoi(data.substr(0, pos1)));
            msg.header.payloadLength = static_cast<uint16_t>(std::stoi(data.substr(pos1 + 1, pos2 - pos1 - 1)));
            msg.header.sequenceNumber = static_cast<uint32_t>(std::stoi(data.substr(pos2 + 1, pos3 - pos2 - 1)));
            
            // Payload is everything after the 3rd delimiter (may contain '|')
            msg.payload = data.substr(pos3 + 1);
        } catch (...) {
            msg.header.type = MessageType::UNKNOWN;
        }
        
        return msg;
    }
};

// User data structure
struct UserData {
    std::string username;
    std::string passwordHash;
    UserRole role;
    ProficiencyLevel level;
    int score;
    
    UserData() : role(UserRole::STUDENT), level(ProficiencyLevel::BEGINNER), score(0) {}
};

// Session data for connected clients
struct SessionData {
    SOCKET socket;
    std::string username;
    UserRole role;
    ConnectionState state;
    std::chrono::steady_clock::time_point lastActivity;
    std::string receiveBuffer;
    std::string sendBuffer;
    
    SessionData() : socket(INVALID_SOCKET), role(UserRole::STUDENT), 
                    state(ConnectionState::DISCONNECTED) {}
};

#endif // MESSAGE_STRUCTS_HPP

