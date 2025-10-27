#ifndef SERVER_HPP
#define SERVER_HPP

#include "../../include/common.hpp"
#include "../../include/message_structs.hpp"
#include "../protocol/Protocol.hpp"
#include "../protocol/Network.hpp"
#include "../utils/Logger.hpp"
#include "../db/Database.hpp"
#include "ClientHandler.hpp"

// Server class using I/O multiplexing for handling multiple clients
class Server {
public:
    Server();
    ~Server();
    
    // Initialize server with configuration
    bool initialize(const std::string& address, int port);
    
    // Start server main loop
    void run();
    
    // Stop server gracefully
    void stop();
    
    // Get server status
    bool isRunning() const { return running_; }

private:
    // Handle new client connections
    void handleNewConnection();
    
    // Handle client data reception
    void handleClientData(SOCKET clientSocket);
    
    // Handle client disconnection
    void handleClientDisconnect(SOCKET clientSocket);
    
    // Process received message
    void processMessage(SOCKET clientSocket, const Message& message);
    
    // Send message to client
    bool sendMessage(SOCKET clientSocket, const Message& message);
    
    // Clean up disconnected clients
    void cleanupClients();
    
    SOCKET listenSocket_;
    std::string serverAddress_;
    int serverPort_;
    bool running_;
    
    std::map<SOCKET, std::unique_ptr<ClientHandler>> clients_;
    std::mutex clientsMutex_;
    
    Protocol protocol_;
    
    #ifdef _WIN32
        fd_set masterSet_;
    #else
        std::vector<struct pollfd> pollFds_;
    #endif
};

#endif // SERVER_HPP

