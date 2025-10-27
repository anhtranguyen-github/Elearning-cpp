#include <iostream>
#include <string>
#include <iomanip>
#include "include/message_structs.hpp"

int main() {
    std::cout << "=== Message Serialization/Deserialization Test ===" << std::endl;
    
    // Create a LOGIN_SUCCESS message
    std::string payload = "3|1|0";
    Message msg(MessageType::LOGIN_SUCCESS, payload);
    
    std::cout << "Original message:" << std::endl;
    std::cout << "  Type: " << static_cast<int>(msg.header.type) << std::endl;
    std::cout << "  PayloadLength: " << msg.header.payloadLength << std::endl;
    std::cout << "  Payload: '" << msg.payload << "'" << std::endl;
    std::cout << "  Payload.length(): " << msg.payload.length() << std::endl;
    
    // Serialize
    std::string serialized = msg.serialize();
    std::cout << "\nSerialized: '" << serialized << "'" << std::endl;
    std::cout << "Serialized length: " << serialized.length() << std::endl;
    std::cout << "Serialized hex: ";
    for (unsigned char c : serialized) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)c << " ";
    }
    std::cout << std::dec << std::endl;
    
    // Remove newline (simulate extractMessages behavior)
    std::string messageData = serialized.substr(0, serialized.find('\n'));
    std::cout << "\nAfter removing \\n: '" << messageData << "'" << std::endl;
    std::cout << "Length: " << messageData.length() << std::endl;
    
    // Deserialize
    Message deserialized = Message::deserialize(messageData);
    std::cout << "\nDeserialized message:" << std::endl;
    std::cout << "  Type: " << static_cast<int>(deserialized.header.type) << std::endl;
    std::cout << "  PayloadLength: " << deserialized.header.payloadLength << std::endl;
    std::cout << "  Payload: '" << deserialized.payload << "'" << std::endl;
    std::cout << "  Payload.length(): " << deserialized.payload.length() << std::endl;
    
    // Validation check
    std::cout << "\nValidation:" << std::endl;
    std::cout << "  Type == UNKNOWN? " << (deserialized.header.type == MessageType::UNKNOWN ? "YES" : "NO") << std::endl;
    std::cout << "  Length match? " << (deserialized.payload.length() == deserialized.header.payloadLength ? "YES" : "NO") << std::endl;
    if (deserialized.payload.length() != deserialized.header.payloadLength) {
        std::cout << "  ERROR: Expected " << deserialized.header.payloadLength 
                  << " but got " << deserialized.payload.length() << std::endl;
    }
    
    return 0;
}

