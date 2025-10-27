// Test program for message format validation

#include "../include/common.hpp"
#include "../include/message_structs.hpp"
#include <iostream>
#include <cassert>

void testMessageSerialization() {
    std::cout << "Testing message serialization..." << std::endl;
    
    // Create a test message
    Message msg(MessageType::LOGIN_REQUEST, "testuser|password123");
    
    // Serialize
    std::string serialized = msg.serialize();
    std::cout << "Serialized: " << serialized;
    
    // Deserialize
    Message deserialized = Message::deserialize(serialized.substr(0, serialized.length() - 1));
    
    // Validate
    assert(deserialized.header.type == MessageType::LOGIN_REQUEST);
    assert(deserialized.payload == "testuser|password123");
    
    std::cout << "✓ Message serialization test passed" << std::endl;
}

void testMessageTypes() {
    std::cout << "Testing message types..." << std::endl;
    
    // Test various message types
    Message regMsg(MessageType::REGISTER_REQUEST);
    assert(regMsg.header.type == MessageType::REGISTER_REQUEST);
    
    Message loginMsg(MessageType::LOGIN_REQUEST);
    assert(loginMsg.header.type == MessageType::LOGIN_REQUEST);
    
    Message chatMsg(MessageType::CHAT_MESSAGE);
    assert(chatMsg.header.type == MessageType::CHAT_MESSAGE);
    
    std::cout << "✓ Message types test passed" << std::endl;
}

void testPayloadHandling() {
    std::cout << "Testing payload handling..." << std::endl;
    
    // Test with different payload sizes
    std::string smallPayload = "test";
    std::string mediumPayload = std::string(100, 'x');
    std::string largePayload = std::string(1000, 'y');
    
    Message smallMsg(MessageType::HEARTBEAT_REQUEST, smallPayload);
    assert(smallMsg.header.payloadLength == smallPayload.length());
    
    Message mediumMsg(MessageType::CHAT_MESSAGE, mediumPayload);
    assert(mediumMsg.header.payloadLength == mediumPayload.length());
    
    Message largeMsg(MessageType::GET_LESSON_CONTENT_RESPONSE, largePayload);
    assert(largeMsg.header.payloadLength == largePayload.length());
    
    std::cout << "✓ Payload handling test passed" << std::endl;
}

void testRoundTrip() {
    std::cout << "Testing round-trip serialization..." << std::endl;
    
    // Create messages with various types and payloads
    std::vector<Message> messages = {
        Message(MessageType::LOGIN_REQUEST, "user|pass"),
        Message(MessageType::REGISTER_REQUEST, "newuser|newpass|1"),
        Message(MessageType::CHAT_MESSAGE, "recipient|Hello World!"),
        Message(MessageType::HEARTBEAT_REQUEST, "ping"),
        Message(MessageType::GET_LESSON_LIST_REQUEST, "")
    };
    
    for (const auto& original : messages) {
        std::string serialized = original.serialize();
        Message deserialized = Message::deserialize(serialized.substr(0, serialized.length() - 1));
        
        assert(original.header.type == deserialized.header.type);
        assert(original.payload == deserialized.payload);
        assert(original.header.payloadLength == deserialized.header.payloadLength);
    }
    
    std::cout << "✓ Round-trip test passed" << std::endl;
}

int main() {
    std::cout << "=== Message Format Tests ===" << std::endl;
    std::cout << std::endl;
    
    try {
        testMessageSerialization();
        testMessageTypes();
        testPayloadHandling();
        testRoundTrip();
        
        std::cout << std::endl;
        std::cout << "=== All tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}

