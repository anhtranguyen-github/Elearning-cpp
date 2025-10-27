// Test program for network connection functionality

#include "../include/common.hpp"
#include "../src/protocol/Network.hpp"
#include "../src/utils/Logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void testSocketCreation() {
    std::cout << "Testing socket creation..." << std::endl;
    
    if (!Network::initialize()) {
        std::cerr << "Failed to initialize network" << std::endl;
        return;
    }
    
    SOCKET sock = Network::createSocket();
    assert(Network::isValidSocket(sock));
    
    Network::closeSocket(sock);
    Network::cleanup();
    
    std::cout << "✓ Socket creation test passed" << std::endl;
}

void testSocketOptions() {
    std::cout << "Testing socket options..." << std::endl;
    
    Network::initialize();
    SOCKET sock = Network::createSocket();
    
    // Test setting socket to non-blocking
    bool result = Network::setNonBlocking(sock);
    assert(result);
    
    // Test socket option setting
    result = Network::setSocketOption(sock, SOL_SOCKET, SO_REUSEADDR, 1);
    assert(result);
    
    Network::closeSocket(sock);
    Network::cleanup();
    
    std::cout << "✓ Socket options test passed" << std::endl;
}

void testBindAndListen() {
    std::cout << "Testing bind and listen..." << std::endl;
    
    Network::initialize();
    SOCKET sock = Network::createSocket();
    
    // Bind to a random available port
    bool bound = Network::bindSocket(sock, "127.0.0.1", 0);
    assert(bound);
    
    bool listening = Network::listenSocket(sock);
    assert(listening);
    
    Network::closeSocket(sock);
    Network::cleanup();
    
    std::cout << "✓ Bind and listen test passed" << std::endl;
}

void serverThread(int port, bool* serverReady) {
    Network::initialize();
    
    SOCKET listenSock = Network::createSocket();
    if (!Network::bindSocket(listenSock, "127.0.0.1", port)) {
        std::cerr << "Server: Bind failed" << std::endl;
        return;
    }
    
    if (!Network::listenSocket(listenSock)) {
        std::cerr << "Server: Listen failed" << std::endl;
        return;
    }
    
    Network::setNonBlocking(listenSock);
    *serverReady = true;
    
    // Wait for connection with timeout
    auto startTime = std::chrono::steady_clock::now();
    const int timeoutSeconds = 5;
    
    while (true) {
        std::string clientAddr;
        int clientPort;
        SOCKET clientSock = Network::acceptConnection(listenSock, clientAddr, clientPort);
        
        if (Network::isValidSocket(clientSock)) {
            std::cout << "Server: Accepted connection from " << clientAddr << std::endl;
            
            // Send a test message
            const char* msg = "Hello from server\n";
            Network::sendData(clientSock, msg, strlen(msg));
            
            // Receive response
            char buffer[256];
            int bytesReceived = Network::receiveData(clientSock, buffer, sizeof(buffer) - 1);
            if (bytesReceived > 0) {
                buffer[bytesReceived] = '\0';
                std::cout << "Server received: " << buffer;
            }
            
            Network::closeSocket(clientSock);
            break;
        }
        
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - startTime).count();
        
        if (elapsed > timeoutSeconds) {
            std::cerr << "Server: Accept timeout" << std::endl;
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    Network::closeSocket(listenSock);
    Network::cleanup();
}

void testClientServerConnection() {
    std::cout << "Testing client-server connection..." << std::endl;
    
    const int TEST_PORT = 9999;
    bool serverReady = false;
    
    // Start server in separate thread
    std::thread server(serverThread, TEST_PORT, &serverReady);
    
    // Wait for server to be ready
    while (!serverReady) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Client connection
    Network::initialize();
    SOCKET clientSock = Network::createSocket();
    
    if (Network::connectToServer(clientSock, "127.0.0.1", TEST_PORT)) {
        std::cout << "Client: Connected to server" << std::endl;
        
        // Receive from server
        char buffer[256];
        int bytesReceived = Network::receiveData(clientSock, buffer, sizeof(buffer) - 1);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << "Client received: " << buffer;
        }
        
        // Send response
        const char* msg = "Hello from client\n";
        Network::sendData(clientSock, msg, strlen(msg));
        
        Network::closeSocket(clientSock);
        std::cout << "✓ Client-server connection test passed" << std::endl;
    } else {
        std::cerr << "Client: Connection failed" << std::endl;
    }
    
    Network::cleanup();
    server.join();
}

int main() {
    std::cout << "=== Network Connection Tests ===" << std::endl;
    std::cout << std::endl;
    
    // Initialize logger
    Logger::getInstance().initialize("logs/test.log", LogLevel::INFO);
    
    try {
        testSocketCreation();
        testSocketOptions();
        testBindAndListen();
        testClientServerConnection();
        
        std::cout << std::endl;
        std::cout << "=== All network tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}

