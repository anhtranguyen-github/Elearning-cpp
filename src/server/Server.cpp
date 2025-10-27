#include "Server.hpp"

Server::Server() 
    : listenSocket_(INVALID_SOCKET), serverPort_(0), running_(false) {
}

Server::~Server() {
    stop();
}

bool Server::initialize(const std::string& address, int port) {
    if (!Network::initialize()) {
        Logger::getInstance().error("Failed to initialize network subsystem");
        return false;
    }
    
    serverAddress_ = address;
    serverPort_ = port;
    
    // Create listen socket
    listenSocket_ = Network::createSocket();
    if (!Network::isValidSocket(listenSocket_)) {
        return false;
    }
    
    // Bind socket
    if (!Network::bindSocket(listenSocket_, serverAddress_, serverPort_)) {
        Network::closeSocket(listenSocket_);
        return false;
    }
    
    // Set to non-blocking mode
    if (!Network::setNonBlocking(listenSocket_)) {
        Logger::getInstance().error("Failed to set listen socket to non-blocking");
        Network::closeSocket(listenSocket_);
        return false;
    }
    
    // Start listening
    if (!Network::listenSocket(listenSocket_)) {
        Network::closeSocket(listenSocket_);
        return false;
    }
    
    Logger::getInstance().info("Server initialized on " + serverAddress_ + ":" + std::to_string(serverPort_));
    return true;
}

void Server::run() {
    running_ = true;
    Logger::getInstance().info("Server started, entering main loop");
    
    #ifdef _WIN32
    // Windows implementation using select()
    FD_ZERO(&masterSet_);
    FD_SET(listenSocket_, &masterSet_);
    
    while (running_) {
        fd_set readSet = masterSet_;
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int activity = select(0, &readSet, NULL, NULL, &timeout);
        
        if (activity == SOCKET_ERROR) {
            Logger::getInstance().error("select() failed: " + Network::getLastError());
            break;
        }
        
        // Check listen socket for new connections
        if (FD_ISSET(listenSocket_, &readSet)) {
            handleNewConnection();
        }
        
        // Check client sockets for data
        std::lock_guard<std::mutex> lock(clientsMutex_);
        for (auto it = clients_.begin(); it != clients_.end(); ) {
            SOCKET clientSock = it->first;
            
            if (FD_ISSET(clientSock, &readSet)) {
                handleClientData(clientSock);
            }
            
            // Check for timeout
            if (it->second->isTimedOut(300)) {
                Logger::getInstance().info("Client timed out: " + it->second->getClientInfo());
                FD_CLR(clientSock, &masterSet_);
                Network::closeSocket(clientSock);
                it = clients_.erase(it);
            } else {
                ++it;
            }
        }
        
        cleanupClients();
    }
    
    #else
    // Linux implementation using poll()
    while (running_) {
        pollFds_.clear();
        
        // Add listen socket
        struct pollfd listenPfd;
        listenPfd.fd = listenSocket_;
        listenPfd.events = POLLIN;
        listenPfd.revents = 0;
        pollFds_.push_back(listenPfd);
        
        // Add client sockets
        {
            std::lock_guard<std::mutex> lock(clientsMutex_);
            for (const auto& pair : clients_) {
                struct pollfd pfd;
                pfd.fd = pair.first;
                pfd.events = POLLIN;
                pfd.revents = 0;
                pollFds_.push_back(pfd);
            }
        }
        
        // Poll with 1 second timeout
        int activity = poll(pollFds_.data(), pollFds_.size(), 1000);
        
        if (activity < 0) {
            if (errno != EINTR) {
                Logger::getInstance().error("poll() failed: " + Network::getLastError());
                break;
            }
            continue;
        }
        
        // Check each socket
        for (size_t i = 0; i < pollFds_.size(); ++i) {
            if (pollFds_[i].revents == 0) continue;
            
            if (pollFds_[i].fd == listenSocket_) {
                // New connection
                if (pollFds_[i].revents & POLLIN) {
                    handleNewConnection();
                }
            } else {
                // Client data or disconnect
                SOCKET clientSock = pollFds_[i].fd;
                
                if (pollFds_[i].revents & POLLIN) {
                    handleClientData(clientSock);
                }
                
                if (pollFds_[i].revents & (POLLHUP | POLLERR)) {
                    handleClientDisconnect(clientSock);
                }
            }
        }
        
        // Check for timeouts
        {
            std::lock_guard<std::mutex> lock(clientsMutex_);
            for (auto it = clients_.begin(); it != clients_.end(); ) {
                if (it->second->isTimedOut(300)) {
                    Logger::getInstance().info("Client timed out: " + it->second->getClientInfo());
                    Network::closeSocket(it->first);
                    it = clients_.erase(it);
                } else {
                    ++it;
                }
            }
        }
        
        cleanupClients();
    }
    #endif
    
    Logger::getInstance().info("Server main loop exited");
}

void Server::stop() {
    if (!running_) return;
    
    running_ = false;
    Logger::getInstance().info("Server stopping...");
    
    // Close all client connections
    {
        std::lock_guard<std::mutex> lock(clientsMutex_);
        for (auto& pair : clients_) {
            Network::closeSocket(pair.first);
        }
        clients_.clear();
    }
    
    // Close listen socket
    if (Network::isValidSocket(listenSocket_)) {
        Network::closeSocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;
    }
    
    Network::cleanup();
    Logger::getInstance().info("Server stopped");
}

void Server::handleNewConnection() {
    std::string clientAddress;
    int clientPort;
    
    SOCKET clientSocket = Network::acceptConnection(listenSocket_, clientAddress, clientPort);
    
    if (!Network::isValidSocket(clientSocket)) {
        return;
    }
    
    // Set client socket to non-blocking
    if (!Network::setNonBlocking(clientSocket)) {
        Logger::getInstance().error("Failed to set client socket to non-blocking");
        Network::closeSocket(clientSocket);
        return;
    }
    
    // Create client handler
    std::lock_guard<std::mutex> lock(clientsMutex_);
    clients_[clientSocket] = std::make_unique<ClientHandler>(clientSocket, clientAddress, clientPort);
    
    #ifdef _WIN32
    FD_SET(clientSocket, &masterSet_);
    #endif
    
    Logger::getInstance().info("New client connected: " + clientAddress + ":" + std::to_string(clientPort));
}

void Server::handleClientData(SOCKET clientSocket) {
    char buffer[AppConstants::BUFFER_SIZE];
    int bytesReceived = Network::receiveData(clientSocket, buffer, sizeof(buffer) - 1);
    
    if (bytesReceived <= 0) {
        handleClientDisconnect(clientSocket);
        return;
    }
    
    buffer[bytesReceived] = '\0';
    
    Logger::getInstance().debug("Received " + std::to_string(bytesReceived) + " bytes: " + std::string(buffer, bytesReceived));
    
    // Extract messages (without holding the lock)
    std::string tempBuffer(buffer, bytesReceived);
    std::vector<Message> messages = protocol_.extractMessages(tempBuffer);
    
    Logger::getInstance().debug("Extracted " + std::to_string(messages.size()) + " messages");
    
    // Process each message (processMessage will acquire its own lock)
    for (const auto& msg : messages) {
        processMessage(clientSocket, msg);
    }
}

void Server::handleClientDisconnect(SOCKET clientSocket) {
    Logger::getInstance().info("Client disconnected");
    
    std::lock_guard<std::mutex> lock(clientsMutex_);
    
    #ifdef _WIN32
    FD_CLR(clientSocket, &masterSet_);
    #endif
    
    Network::closeSocket(clientSocket);
    clients_.erase(clientSocket);
}

void Server::processMessage(SOCKET clientSocket, const Message& message) {
    Logger::getInstance().debug("processMessage called for message type " + 
                               std::to_string(static_cast<int>(message.header.type)));
    
    std::lock_guard<std::mutex> lock(clientsMutex_);
    
    auto it = clients_.find(clientSocket);
    if (it == clients_.end()) {
        Logger::getInstance().warning("Client not found in processMessage");
        return;
    }
    
    Logger::getInstance().debug("Calling ClientHandler::processMessage");
    
    // Process message through client handler
    Message response = it->second->processMessage(message);
    
    Logger::getInstance().debug("ClientHandler returned response type " + 
                               std::to_string(static_cast<int>(response.header.type)));
    
    // Send response
    sendMessage(clientSocket, response);
}

bool Server::sendMessage(SOCKET clientSocket, const Message& message) {
    std::string data = protocol_.encodeMessage(message);
    
    Logger::getInstance().debug("Sending message type " + 
                               std::to_string(static_cast<int>(message.header.type)) +
                               " payload: " + message.payload);
    
    int bytesSent = Network::sendData(clientSocket, data.c_str(), data.length());
    
    if (bytesSent <= 0) {
        Logger::getInstance().error("Failed to send message to client");
        return false;
    }
    
    Logger::getInstance().info("Sent message type " + 
                              std::to_string(static_cast<int>(message.header.type)) +
                              " (" + std::to_string(bytesSent) + " bytes)");
    return true;
}

void Server::cleanupClients() {
    // Placeholder for additional cleanup tasks if needed
}

