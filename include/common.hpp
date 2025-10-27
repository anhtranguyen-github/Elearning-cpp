#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <cstdint>
#include <memory>
#include <vector>
#include <map>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <mutex>
#include <algorithm>

// Platform-specific socket headers
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
    #define SOCKET_ERROR_CODE WSAGetLastError()
    #define CLOSE_SOCKET closesocket
#else
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <sys/select.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
    #include <poll.h>
    typedef int SOCKET;
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define SOCKET_ERROR_CODE errno
    #define CLOSE_SOCKET close
#endif

// Application constants
namespace AppConstants {
    constexpr size_t MAX_MESSAGE_SIZE = 8192;
    constexpr size_t BUFFER_SIZE = 16384;
    constexpr int DEFAULT_PORT = 8080;
    constexpr int MAX_PENDING_CONNECTIONS = 10;
    constexpr const char* MESSAGE_DELIMITER = "\n";
}

// User roles
enum class UserRole : uint8_t {
    STUDENT = 1,
    TEACHER = 2,
    ADMIN = 3
};

// English proficiency levels
enum class ProficiencyLevel : uint8_t {
    BEGINNER = 1,
    INTERMEDIATE = 2,
    ADVANCED = 3
};

// Connection states
enum class ConnectionState : uint8_t {
    DISCONNECTED = 0,
    CONNECTING = 1,
    CONNECTED = 2,
    AUTHENTICATED = 3
};

// Utility functions
namespace Utils {
    // Get current timestamp as string
    inline std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        char buffer[100];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
        return std::string(buffer);
    }

    // Set socket to non-blocking mode
    inline bool setNonBlocking(SOCKET sock) {
        #ifdef _WIN32
            u_long mode = 1;
            return ioctlsocket(sock, FIONBIO, &mode) == 0;
        #else
            int flags = fcntl(sock, F_GETFL, 0);
            if (flags == -1) return false;
            return fcntl(sock, F_SETFL, flags | O_NONBLOCK) != -1;
        #endif
    }

    // Initialize socket library (Windows only)
    inline bool initSocketLibrary() {
        #ifdef _WIN32
            WSADATA wsaData;
            return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
        #else
            return true; // No initialization needed on POSIX
        #endif
    }

    // Cleanup socket library (Windows only)
    inline void cleanupSocketLibrary() {
        #ifdef _WIN32
            WSACleanup();
        #endif
    }

    // Trim whitespace from string
    inline std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, last - first + 1);
    }

    // Split string by delimiter
    inline std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
}

#endif // COMMON_HPP

