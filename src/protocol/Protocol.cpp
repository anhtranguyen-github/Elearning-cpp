#include "Protocol.hpp"
#include <iostream>
#include <iomanip>

Protocol::Protocol() : sequenceNumber_(0) {}

std::string Protocol::encodeMessage(const Message& message) {
    return message.serialize();
}

Message Protocol::decodeMessage(const std::string& data) {
    return Message::deserialize(data);
}

std::vector<Message> Protocol::extractMessages(std::string& buffer) {
    std::vector<Message> messages;
    
    std::cout << "[EXTRACT] Buffer size: " << buffer.size() << " bytes" << std::endl;
    std::cout << "[EXTRACT] Buffer hex: ";
    for (unsigned char c : buffer) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)c << " ";
    }
    std::cout << std::dec << std::endl;
    std::cout.flush();
    
    Logger::getInstance().debug("extractMessages: buffer size = " + std::to_string(buffer.size()));
    
    size_t pos = 0;
    while ((pos = buffer.find('\n')) != std::string::npos) {
        std::string messageData = buffer.substr(0, pos);
        buffer.erase(0, pos + 1);
        
        // Remove trailing \r if present (Windows line endings)
        if (!messageData.empty() && messageData.back() == '\r') {
            messageData.pop_back();
        }
        
        std::cout << "[EXTRACT] Found message (length " << messageData.length() << "): '" << messageData << "'" << std::endl;
        std::cout.flush();
        Logger::getInstance().debug("Found message: " + messageData);
        
        if (!messageData.empty()) {
            Message msg = decodeMessage(messageData);
            
            std::cout << "[DECODE] Type=" << (int)msg.header.type 
                      << " HeaderLen=" << msg.header.payloadLength 
                      << " ActualLen=" << msg.payload.length()
                      << " Payload='" << msg.payload << "'" << std::endl;
            std::cout.flush();
            
            Logger::getInstance().debug("Decoded message type: " + 
                                       std::to_string(static_cast<int>(msg.header.type)) +
                                       ", payload length: " + std::to_string(msg.header.payloadLength) +
                                       ", actual payload: " + msg.payload);
            
            Logger::getInstance().debug("About to validate message...");
            bool isValid = validateMessage(msg);
            Logger::getInstance().debug("Validation result: " + std::string(isValid ? "PASS" : "FAIL"));
            
            if (isValid) {
                Logger::getInstance().debug("Message validated successfully");
                messages.push_back(msg);
            } else {
                Logger::getInstance().warning("Invalid message format: " + messageData);
            }
        }
    }
    
    std::cout << "[EXTRACT] Total messages extracted: " << messages.size() << ", remaining buffer: " << buffer.size() << " bytes" << std::endl;
    std::cout.flush();
    
    Logger::getInstance().debug("Extracted " + std::to_string(messages.size()) + " messages, remaining buffer: " + std::to_string(buffer.size()) + " bytes");
    
    // Keep incomplete message in buffer
    return messages;
}

bool Protocol::validateMessage(const Message& message) {
    // Check if message type is valid
    if (message.header.type == MessageType::UNKNOWN) {
        std::cout << "[VALIDATE] FAIL: UNKNOWN message type" << std::endl;
        std::cout.flush();
        Logger::getInstance().debug("Validation failed: UNKNOWN message type");
        return false;
    }
    
    // Check payload length
    if (message.payload.length() != message.header.payloadLength) {
        std::cout << "[VALIDATE] FAIL: Length mismatch - header=" << message.header.payloadLength 
                  << " actual=" << message.payload.length() << " payload='" << message.payload << "'" << std::endl;
        std::cout.flush();
        Logger::getInstance().debug("Validation failed: payload length mismatch - header says " + 
                                   std::to_string(message.header.payloadLength) + 
                                   " but actual is " + std::to_string(message.payload.length()) +
                                   " (payload: '" + message.payload + "')");
        return false;
    }
    
    // Check message size
    if (message.payload.length() > AppConstants::MAX_MESSAGE_SIZE) {
        std::cout << "[VALIDATE] FAIL: Payload too large" << std::endl;
        std::cout.flush();
        Logger::getInstance().debug("Validation failed: payload too large");
        return false;
    }
    
    std::cout << "[VALIDATE] PASS: type=" << static_cast<int>(message.header.type) << std::endl;
    std::cout.flush();
    return true;
}

std::string Protocol::getMessageTypeName(MessageType type) {
    switch (type) {
        case MessageType::REGISTER_REQUEST: return "REGISTER_REQUEST";
        case MessageType::REGISTER_SUCCESS: return "REGISTER_SUCCESS";
        case MessageType::REGISTER_FAILED: return "REGISTER_FAILED";
        case MessageType::LOGIN_REQUEST: return "LOGIN_REQUEST";
        case MessageType::LOGIN_SUCCESS: return "LOGIN_SUCCESS";
        case MessageType::LOGIN_FAILED: return "LOGIN_FAILED";
        case MessageType::LOGOUT_REQUEST: return "LOGOUT_REQUEST";
        case MessageType::LOGOUT_SUCCESS: return "LOGOUT_SUCCESS";
        case MessageType::SET_LEVEL_REQUEST: return "SET_LEVEL_REQUEST";
        case MessageType::SET_LEVEL_SUCCESS: return "SET_LEVEL_SUCCESS";
        case MessageType::SET_LEVEL_FAILED: return "SET_LEVEL_FAILED";
        case MessageType::GET_LESSON_LIST_REQUEST: return "GET_LESSON_LIST_REQUEST";
        case MessageType::GET_LESSON_LIST_RESPONSE: return "GET_LESSON_LIST_RESPONSE";
        case MessageType::GET_LESSON_CONTENT_REQUEST: return "GET_LESSON_CONTENT_REQUEST";
        case MessageType::GET_LESSON_CONTENT_RESPONSE: return "GET_LESSON_CONTENT_RESPONSE";
        case MessageType::SUBMIT_QUIZ_REQUEST: return "SUBMIT_QUIZ_REQUEST";
        case MessageType::SUBMIT_QUIZ_RESPONSE: return "SUBMIT_QUIZ_RESPONSE";
        case MessageType::SUBMIT_EXERCISE_REQUEST: return "SUBMIT_EXERCISE_REQUEST";
        case MessageType::SUBMIT_EXERCISE_RESPONSE: return "SUBMIT_EXERCISE_RESPONSE";
        case MessageType::GAME_START_REQUEST: return "GAME_START_REQUEST";
        case MessageType::GAME_START_RESPONSE: return "GAME_START_RESPONSE";
        case MessageType::GAME_MOVE_REQUEST: return "GAME_MOVE_REQUEST";
        case MessageType::GAME_MOVE_RESPONSE: return "GAME_MOVE_RESPONSE";
        case MessageType::GAME_END_NOTIFICATION: return "GAME_END_NOTIFICATION";
        case MessageType::GET_SCORE_REQUEST: return "GET_SCORE_REQUEST";
        case MessageType::GET_SCORE_RESPONSE: return "GET_SCORE_RESPONSE";
        case MessageType::GET_FEEDBACK_REQUEST: return "GET_FEEDBACK_REQUEST";
        case MessageType::GET_FEEDBACK_RESPONSE: return "GET_FEEDBACK_RESPONSE";
        case MessageType::SEND_FEEDBACK_REQUEST: return "SEND_FEEDBACK_REQUEST";
        case MessageType::SEND_FEEDBACK_SUCCESS: return "SEND_FEEDBACK_SUCCESS";
        case MessageType::CHAT_MESSAGE: return "CHAT_MESSAGE";
        case MessageType::CHAT_MESSAGE_ACK: return "CHAT_MESSAGE_ACK";
        case MessageType::VOICE_CALL_REQUEST: return "VOICE_CALL_REQUEST";
        case MessageType::VOICE_CALL_ACCEPT: return "VOICE_CALL_ACCEPT";
        case MessageType::VOICE_CALL_REJECT: return "VOICE_CALL_REJECT";
        case MessageType::VOICE_CALL_END: return "VOICE_CALL_END";
        case MessageType::ADD_GAME_ITEM_REQUEST: return "ADD_GAME_ITEM_REQUEST";
        case MessageType::ADD_GAME_ITEM_SUCCESS: return "ADD_GAME_ITEM_SUCCESS";
        case MessageType::ADD_GAME_ITEM_FAILED: return "ADD_GAME_ITEM_FAILED";
        case MessageType::HEARTBEAT_REQUEST: return "HEARTBEAT_REQUEST";
        case MessageType::HEARTBEAT_RESPONSE: return "HEARTBEAT_RESPONSE";
        case MessageType::ERROR_MESSAGE: return "ERROR_MESSAGE";
        case MessageType::DISCONNECT_NOTIFICATION: return "DISCONNECT_NOTIFICATION";
        default: return "UNKNOWN";
    }
}

bool Protocol::requiresAuthentication(MessageType type) {
    // Messages that don't require authentication
    switch (type) {
        case MessageType::REGISTER_REQUEST:
        case MessageType::LOGIN_REQUEST:
        case MessageType::HEARTBEAT_REQUEST:
        case MessageType::DISCONNECT_NOTIFICATION:
            return false;
        default:
            return true;
    }
}

uint32_t Protocol::getNextSequenceNumber() {
    std::lock_guard<std::mutex> lock(seqMutex_);
    return ++sequenceNumber_;
}

