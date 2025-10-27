#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include "../../include/common.hpp"
#include "../../include/message_structs.hpp"
#include "../utils/Logger.hpp"

// Protocol handler class for message encoding/decoding and validation
class Protocol {
public:
    Protocol();
    ~Protocol() = default;
    
    // Encode a message to wire format
    std::string encodeMessage(const Message& message);
    
    // Decode a message from wire format
    Message decodeMessage(const std::string& data);
    
    // Extract complete messages from a buffer (handles partial messages)
    // Returns vector of complete messages and updates the buffer
    std::vector<Message> extractMessages(std::string& buffer);
    
    // Validate message format and content
    bool validateMessage(const Message& message);
    
    // Get message type name for logging
    std::string getMessageTypeName(MessageType type);
    
    // Check if message type requires authentication
    bool requiresAuthentication(MessageType type);
    
    // Get next sequence number
    uint32_t getNextSequenceNumber();

private:
    uint32_t sequenceNumber_;
    std::mutex seqMutex_;
};

#endif // PROTOCOL_HPP

