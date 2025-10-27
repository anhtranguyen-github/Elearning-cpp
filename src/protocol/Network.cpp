#include "Network.hpp"

bool Network::initialize() {
    return Utils::initSocketLibrary();
}

void Network::cleanup() {
    Utils::cleanupSocketLibrary();
}

SOCKET Network::createSocket() {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        Logger::getInstance().error("Failed to create socket: " + getLastError());
    }
    return sock;
}

bool Network::bindSocket(SOCKET sock, const std::string& address, int port) {
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    if (address == "0.0.0.0" || address.empty()) {
        serverAddr.sin_addr.s_addr = INADDR_ANY;
    } else {
        #ifdef _WIN32
            InetPtonA(AF_INET, address.c_str(), &serverAddr.sin_addr);
        #else
            inet_pton(AF_INET, address.c_str(), &serverAddr.sin_addr);
        #endif
    }
    
    // Set socket option to reuse address
    setSocketOption(sock, SOL_SOCKET, SO_REUSEADDR, 1);
    
    if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        Logger::getInstance().error("Failed to bind socket: " + getLastError());
        return false;
    }
    
    Logger::getInstance().info("Socket bound to " + address + ":" + std::to_string(port));
    return true;
}

bool Network::listenSocket(SOCKET sock, int backlog) {
    if (listen(sock, backlog) == SOCKET_ERROR) {
        Logger::getInstance().error("Failed to listen on socket: " + getLastError());
        return false;
    }
    
    Logger::getInstance().info("Socket listening with backlog: " + std::to_string(backlog));
    return true;
}

SOCKET Network::acceptConnection(SOCKET listenSock, std::string& clientAddress, int& clientPort) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    
    SOCKET clientSock = accept(listenSock, (struct sockaddr*)&clientAddr, &clientAddrLen);
    
    if (clientSock == INVALID_SOCKET) {
        int errorCode = SOCKET_ERROR_CODE;
        #ifdef _WIN32
            if (errorCode != WSAEWOULDBLOCK) {
        #else
            if (errorCode != EWOULDBLOCK && errorCode != EAGAIN) {
        #endif
                Logger::getInstance().error("Accept failed: " + getLastError());
            }
        return INVALID_SOCKET;
    }
    
    // Get client address and port
    char addrStr[INET_ADDRSTRLEN];
    #ifdef _WIN32
        InetNtopA(AF_INET, &clientAddr.sin_addr, addrStr, INET_ADDRSTRLEN);
    #else
        inet_ntop(AF_INET, &clientAddr.sin_addr, addrStr, INET_ADDRSTRLEN);
    #endif
    
    clientAddress = std::string(addrStr);
    clientPort = ntohs(clientAddr.sin_port);
    
    Logger::getInstance().info("Accepted connection from " + clientAddress + ":" + std::to_string(clientPort));
    return clientSock;
}

bool Network::connectToServer(SOCKET sock, const std::string& address, int port, int timeoutSec) {
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    #ifdef _WIN32
        InetPtonA(AF_INET, address.c_str(), &serverAddr.sin_addr);
    #else
        inet_pton(AF_INET, address.c_str(), &serverAddr.sin_addr);
    #endif
    
    Logger::getInstance().info("Connecting to " + address + ":" + std::to_string(port));
    
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        Logger::getInstance().error("Failed to connect: " + getLastError());
        return false;
    }
    
    Logger::getInstance().info("Connected to server");
    return true;
}

int Network::sendData(SOCKET sock, const char* data, size_t length) {
    int bytesSent = send(sock, data, static_cast<int>(length), 0);
    
    if (bytesSent == SOCKET_ERROR) {
        int errorCode = SOCKET_ERROR_CODE;
        #ifdef _WIN32
            if (errorCode != WSAEWOULDBLOCK) {
        #else
            if (errorCode != EWOULDBLOCK && errorCode != EAGAIN) {
        #endif
                Logger::getInstance().error("Send failed: " + getLastError());
            }
        return -1;
    }
    
    return bytesSent;
}

int Network::receiveData(SOCKET sock, char* buffer, size_t bufferSize) {
    int bytesReceived = recv(sock, buffer, static_cast<int>(bufferSize), 0);
    
    if (bytesReceived == SOCKET_ERROR) {
        int errorCode = SOCKET_ERROR_CODE;
        #ifdef _WIN32
            if (errorCode != WSAEWOULDBLOCK) {
        #else
            if (errorCode != EWOULDBLOCK && errorCode != EAGAIN) {
        #endif
                Logger::getInstance().error("Receive failed: " + getLastError());
            }
        return -1;
    }
    
    return bytesReceived;
}

void Network::closeSocket(SOCKET sock) {
    if (isValidSocket(sock)) {
        CLOSE_SOCKET(sock);
        Logger::getInstance().debug("Socket closed");
    }
}

bool Network::setSocketOption(SOCKET sock, int level, int optname, int optval) {
    #ifdef _WIN32
        char optvalChar = static_cast<char>(optval);
        return setsockopt(sock, level, optname, &optvalChar, sizeof(optvalChar)) == 0;
    #else
        return setsockopt(sock, level, optname, &optval, sizeof(optval)) == 0;
    #endif
}

bool Network::setNonBlocking(SOCKET sock) {
    return Utils::setNonBlocking(sock);
}

bool Network::setBlocking(SOCKET sock) {
    #ifdef _WIN32
        u_long mode = 0;
        return ioctlsocket(sock, FIONBIO, &mode) == 0;
    #else
        int flags = fcntl(sock, F_GETFL, 0);
        if (flags == -1) return false;
        return fcntl(sock, F_SETFL, flags & ~O_NONBLOCK) != -1;
    #endif
}

bool Network::isValidSocket(SOCKET sock) {
    return sock != INVALID_SOCKET;
}

std::string Network::getLastError() {
    #ifdef _WIN32
        int error = WSAGetLastError();
        char buffer[256];
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL, error, 0, buffer, sizeof(buffer), NULL);
        return std::string(buffer) + " (Code: " + std::to_string(error) + ")";
    #else
        return std::string(strerror(errno)) + " (Code: " + std::to_string(errno) + ")";
    #endif
}

