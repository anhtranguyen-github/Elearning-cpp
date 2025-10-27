#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "../../include/common.hpp"
#include "../../include/message_structs.hpp"
#include "../utils/Logger.hpp"

// Network utility class for socket operations
class Network {
public:
    // Initialize network subsystem (Windows WSA)
    static bool initialize();
    
    // Cleanup network subsystem
    static void cleanup();
    
    // Create a TCP socket
    static SOCKET createSocket();
    
    // Bind socket to address and port
    static bool bindSocket(SOCKET sock, const std::string& address, int port);
    
    // Listen on socket
    static bool listenSocket(SOCKET sock, int backlog = AppConstants::MAX_PENDING_CONNECTIONS);
    
    // Accept incoming connection
    static SOCKET acceptConnection(SOCKET listenSock, std::string& clientAddress, int& clientPort);
    
    // Connect to server
    static bool connectToServer(SOCKET sock, const std::string& address, int port, int timeoutSec = 10);
    
    // Send data through socket
    static int sendData(SOCKET sock, const char* data, size_t length);
    
    // Receive data from socket
    static int receiveData(SOCKET sock, char* buffer, size_t bufferSize);
    
    // Close socket properly
    static void closeSocket(SOCKET sock);
    
    // Set socket options
    static bool setSocketOption(SOCKET sock, int level, int optname, int optval);
    
    // Set socket to non-blocking mode
    static bool setNonBlocking(SOCKET sock);
    
    // Set socket to blocking mode
    static bool setBlocking(SOCKET sock);
    
    // Check if socket is valid
    static bool isValidSocket(SOCKET sock);
    
    // Get last socket error
    static std::string getLastError();
    
private:
    Network() = default;
};

#endif // NETWORK_HPP

