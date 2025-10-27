#ifndef CLIENT_HANDLER_HPP
#define CLIENT_HANDLER_HPP

#include "../../include/common.hpp"
#include "../../include/message_structs.hpp"
#include "../protocol/Protocol.hpp"
#include "../utils/Logger.hpp"
#include "../utils/Parser.hpp"
#include "../db/Database.hpp"

// Client handler class for managing individual client state and message processing
class ClientHandler {
public:
    ClientHandler(SOCKET socket, const std::string& address, int port);
    ~ClientHandler();
    
    // Process incoming message
    Message processMessage(const Message& message);
    
    // Get client socket
    SOCKET getSocket() const { return socket_; }
    
    // Get client info
    std::string getClientInfo() const;
    
    // Check if authenticated
    bool isAuthenticated() const { return authenticated_; }
    
    // Get username
    std::string getUsername() const { return username_; }
    
    // Get user role
    UserRole getRole() const { return role_; }
    
    // Update last activity time
    void updateActivity();
    
    // Check if session timed out
    bool isTimedOut(int timeoutSeconds) const;

private:
    // Message handlers
    Message handleRegisterRequest(const Message& message);
    Message handleLoginRequest(const Message& message);
    Message handleLogoutRequest(const Message& message);
    Message handleSetLevelRequest(const Message& message);
    Message handleGetLessonListRequest(const Message& message);
    Message handleGetLessonContentRequest(const Message& message);
    Message handleSubmitQuizRequest(const Message& message);
    Message handleSubmitExerciseRequest(const Message& message);
    Message handleGameStartRequest(const Message& message);
    Message handleGameMoveRequest(const Message& message);
    Message handleGetScoreRequest(const Message& message);
    Message handleGetFeedbackRequest(const Message& message);
    Message handleSendFeedbackRequest(const Message& message);
    Message handleChatMessage(const Message& message);
    Message handleVoiceCallRequest(const Message& message);
    Message handleAddGameItemRequest(const Message& message);
    Message handleHeartbeatRequest(const Message& message);
    
    // Create error response
    Message createErrorResponse(ErrorCode code, const std::string& description);
    
    SOCKET socket_;
    std::string clientAddress_;
    int clientPort_;
    
    bool authenticated_;
    std::string username_;
    UserRole role_;
    ProficiencyLevel level_;
    
    std::chrono::steady_clock::time_point lastActivity_;
};

#endif // CLIENT_HANDLER_HPP

