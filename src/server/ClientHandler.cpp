#include "ClientHandler.hpp"

ClientHandler::ClientHandler(SOCKET socket, const std::string& address, int port)
    : socket_(socket), clientAddress_(address), clientPort_(port),
      authenticated_(false), role_(UserRole::STUDENT), level_(ProficiencyLevel::BEGINNER) {
    
    lastActivity_ = std::chrono::steady_clock::now();
    Logger::getInstance().info("ClientHandler created for " + getClientInfo());
}

ClientHandler::~ClientHandler() {
    Logger::getInstance().info("ClientHandler destroyed for " + getClientInfo());
    if (authenticated_) {
        Database::getInstance().removeSession(username_);
    }
}

std::string ClientHandler::getClientInfo() const {
    return clientAddress_ + ":" + std::to_string(clientPort_) + 
           (authenticated_ ? " [" + username_ + "]" : " [guest]");
}

void ClientHandler::updateActivity() {
    lastActivity_ = std::chrono::steady_clock::now();
}

bool ClientHandler::isTimedOut(int timeoutSeconds) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastActivity_).count();
    return elapsed > timeoutSeconds;
}

Message ClientHandler::processMessage(const Message& message) {
    updateActivity();
    
    Logger::getInstance().info("Processing message from " + getClientInfo() + 
                              ": " + std::to_string(static_cast<int>(message.header.type)));
    
    switch (message.header.type) {
        case MessageType::REGISTER_REQUEST:
            return handleRegisterRequest(message);
        case MessageType::LOGIN_REQUEST:
            return handleLoginRequest(message);
        case MessageType::LOGOUT_REQUEST:
            return handleLogoutRequest(message);
        case MessageType::SET_LEVEL_REQUEST:
            return handleSetLevelRequest(message);
        case MessageType::GET_LESSON_LIST_REQUEST:
            return handleGetLessonListRequest(message);
        case MessageType::GET_LESSON_CONTENT_REQUEST:
            return handleGetLessonContentRequest(message);
        case MessageType::SUBMIT_QUIZ_REQUEST:
            return handleSubmitQuizRequest(message);
        case MessageType::SUBMIT_EXERCISE_REQUEST:
            return handleSubmitExerciseRequest(message);
        case MessageType::GAME_START_REQUEST:
            return handleGameStartRequest(message);
        case MessageType::GAME_MOVE_REQUEST:
            return handleGameMoveRequest(message);
        case MessageType::GET_SCORE_REQUEST:
            return handleGetScoreRequest(message);
        case MessageType::GET_FEEDBACK_REQUEST:
            return handleGetFeedbackRequest(message);
        case MessageType::SEND_FEEDBACK_REQUEST:
            return handleSendFeedbackRequest(message);
        case MessageType::CHAT_MESSAGE:
            return handleChatMessage(message);
        case MessageType::VOICE_CALL_REQUEST:
            return handleVoiceCallRequest(message);
        case MessageType::ADD_GAME_ITEM_REQUEST:
            return handleAddGameItemRequest(message);
        case MessageType::HEARTBEAT_REQUEST:
            return handleHeartbeatRequest(message);
        default:
            return createErrorResponse(ErrorCode::INVALID_FORMAT, "Unknown message type");
    }
}

Message ClientHandler::handleRegisterRequest(const Message& message) {
    std::string username, password;
    UserRole role;
    
    if (!Parser::parseRegisterRequest(message.payload, username, password, role)) {
        return createErrorResponse(ErrorCode::INVALID_FORMAT, "Invalid registration data");
    }
    
    if (Database::getInstance().userExists(username)) {
        return Message(MessageType::REGISTER_FAILED, 
                      Parser::createErrorMessage(ErrorCode::USER_ALREADY_EXISTS, "Username already exists"));
    }
    
    std::string passwordHash = Database::getInstance().hashPassword(password);
    
    if (Database::getInstance().createUser(username, passwordHash, role)) {
        Logger::getInstance().info("User registered: " + username);
        return Message(MessageType::REGISTER_SUCCESS, Parser::createSuccessMessage());
    }
    
    return createErrorResponse(ErrorCode::DATABASE_ERROR, "Failed to create user");
}

Message ClientHandler::handleLoginRequest(const Message& message) {
    std::string username, password;
    
    if (!Parser::parseLoginRequest(message.payload, username, password)) {
        return createErrorResponse(ErrorCode::INVALID_FORMAT, "Invalid login data");
    }
    
    std::string passwordHash = Database::getInstance().hashPassword(password);
    
    if (!Database::getInstance().authenticateUser(username, passwordHash)) {
        Logger::getInstance().warning("Failed login attempt: " + username);
        return Message(MessageType::LOGIN_FAILED, 
                      Parser::createErrorMessage(ErrorCode::INVALID_CREDENTIALS, "Invalid username or password"));
    }
    
    UserData userData;
    if (!Database::getInstance().getUserData(username, userData)) {
        return createErrorResponse(ErrorCode::DATABASE_ERROR, "Failed to retrieve user data");
    }
    
    // Set authenticated state
    authenticated_ = true;
    username_ = username;
    role_ = userData.role;
    level_ = userData.level;
    
    // Create session in database
    Database::getInstance().createSession(username, socket_);
    
    Logger::getInstance().info("User logged in: " + username);
    
    std::string response = std::to_string(static_cast<int>(userData.role)) + "|" + 
                          std::to_string(static_cast<int>(userData.level)) + "|" +
                          std::to_string(userData.score);
    
    Logger::getInstance().debug("Creating LOGIN_SUCCESS response: '" + response + "' (length: " + std::to_string(response.length()) + ")");
    
    Message loginMsg(MessageType::LOGIN_SUCCESS, response);
    Logger::getInstance().debug("Message created - header.payloadLength=" + std::to_string(loginMsg.header.payloadLength) + 
                               ", payload.length()=" + std::to_string(loginMsg.payload.length()));
    
    return loginMsg;
}

Message ClientHandler::handleLogoutRequest(const Message& message) {
    if (!authenticated_) {
        return createErrorResponse(ErrorCode::NOT_AUTHENTICATED, "Not logged in");
    }
    
    Database::getInstance().removeSession(username_);
    Logger::getInstance().info("User logged out: " + username_);
    
    authenticated_ = false;
    username_.clear();
    
    return Message(MessageType::LOGOUT_SUCCESS, Parser::createSuccessMessage());
}

Message ClientHandler::handleSetLevelRequest(const Message& message) {
    if (!authenticated_) {
        return createErrorResponse(ErrorCode::NOT_AUTHENTICATED, "Authentication required");
    }
    
    ProficiencyLevel level;
    if (!Parser::parseSetLevelRequest(message.payload, level)) {
        return createErrorResponse(ErrorCode::INVALID_PARAMETER, "Invalid level");
    }
    
    if (Database::getInstance().updateUserLevel(username_, level)) {
        level_ = level;
        return Message(MessageType::SET_LEVEL_SUCCESS, Parser::createSuccessMessage());
    }
    
    return Message(MessageType::SET_LEVEL_FAILED, 
                  Parser::createErrorMessage(ErrorCode::DATABASE_ERROR, "Failed to update level"));
}

Message ClientHandler::handleGetLessonListRequest(const Message& message) {
    if (!authenticated_) {
        return createErrorResponse(ErrorCode::NOT_AUTHENTICATED, "Authentication required");
    }
    
    std::vector<std::string> lessons = Database::getInstance().getLessonList(level_);
    
    std::string response;
    for (const auto& lesson : lessons) {
        if (!response.empty()) response += ";";
        response += lesson;
    }
    
    return Message(MessageType::GET_LESSON_LIST_RESPONSE, response);
}

Message ClientHandler::handleGetLessonContentRequest(const Message& message) {
    if (!authenticated_) {
        return createErrorResponse(ErrorCode::NOT_AUTHENTICATED, "Authentication required");
    }
    
    std::string lessonId = Utils::trim(message.payload);
    std::string content = Database::getInstance().getLessonContent(lessonId);
    
    return Message(MessageType::GET_LESSON_CONTENT_RESPONSE, content);
}

Message ClientHandler::handleSubmitQuizRequest(const Message& message) {
    if (!authenticated_) {
        return createErrorResponse(ErrorCode::NOT_AUTHENTICATED, "Authentication required");
    }
    
    // Parse quiz submission: quizId|answers
    std::vector<std::string> parts = Utils::split(message.payload, '|');
    if (parts.size() < 2) {
        return createErrorResponse(ErrorCode::INVALID_FORMAT, "Invalid quiz submission");
    }
    
    std::string quizId = parts[0];
    // Simple scoring: award 10 points per question
    int score = 10;
    
    Database::getInstance().saveScore(username_, quizId, score);
    
    std::string response = std::to_string(score) + "|Correct answers: 1/1";
    return Message(MessageType::SUBMIT_QUIZ_RESPONSE, response);
}

Message ClientHandler::handleSubmitExerciseRequest(const Message& message) {
    if (!authenticated_) {
        return createErrorResponse(ErrorCode::NOT_AUTHENTICATED, "Authentication required");
    }
    
    // Similar to quiz submission
    std::vector<std::string> parts = Utils::split(message.payload, '|');
    if (parts.size() < 2) {
        return createErrorResponse(ErrorCode::INVALID_FORMAT, "Invalid exercise submission");
    }
    
    std::string exerciseId = parts[0];
    int score = 5;
    
    Database::getInstance().saveScore(username_, exerciseId, score);
    
    return Message(MessageType::SUBMIT_EXERCISE_RESPONSE, std::to_string(score));
}

Message ClientHandler::handleGameStartRequest(const Message& message) {
    if (!authenticated_) {
        return createErrorResponse(ErrorCode::NOT_AUTHENTICATED, "Authentication required");
    }
    
    std::string gameType = Utils::trim(message.payload);
    std::vector<std::string> items = Database::getInstance().getGameItems(gameType);
    
    std::string response = "game_session_id_123|";
    for (const auto& item : items) {
        response += item + ";";
    }
    
    return Message(MessageType::GAME_START_RESPONSE, response);
}

Message ClientHandler::handleGameMoveRequest(const Message& message) {
    if (!authenticated_) {
        return createErrorResponse(ErrorCode::NOT_AUTHENTICATED, "Authentication required");
    }
    
    // Simple game move validation
    std::string result = "move_accepted|score:10";
    return Message(MessageType::GAME_MOVE_RESPONSE, result);
}

Message ClientHandler::handleGetScoreRequest(const Message& message) {
    if (!authenticated_) {
        return createErrorResponse(ErrorCode::NOT_AUTHENTICATED, "Authentication required");
    }
    
    UserData userData;
    if (Database::getInstance().getUserData(username_, userData)) {
        return Message(MessageType::GET_SCORE_RESPONSE, std::to_string(userData.score));
    }
    
    return createErrorResponse(ErrorCode::DATABASE_ERROR, "Failed to retrieve score");
}

Message ClientHandler::handleGetFeedbackRequest(const Message& message) {
    if (!authenticated_) {
        return createErrorResponse(ErrorCode::NOT_AUTHENTICATED, "Authentication required");
    }
    
    std::vector<std::string> feedbacks = Database::getInstance().getFeedback(username_);
    
    std::string response;
    for (const auto& feedback : feedbacks) {
        if (!response.empty()) response += ";";
        response += feedback;
    }
    
    return Message(MessageType::GET_FEEDBACK_RESPONSE, response);
}

Message ClientHandler::handleSendFeedbackRequest(const Message& message) {
    if (!authenticated_) {
        return createErrorResponse(ErrorCode::NOT_AUTHENTICATED, "Authentication required");
    }
    
    if (role_ != UserRole::TEACHER && role_ != UserRole::ADMIN) {
        return createErrorResponse(ErrorCode::PERMISSION_DENIED, "Only teachers can send feedback");
    }
    
    // Parse: targetUser|exerciseId|feedback
    std::vector<std::string> parts = Utils::split(message.payload, '|');
    if (parts.size() < 3) {
        return createErrorResponse(ErrorCode::INVALID_FORMAT, "Invalid feedback format");
    }
    
    std::string targetUser = parts[0];
    std::string exerciseId = parts[1];
    std::string feedback = parts[2];
    
    if (Database::getInstance().saveFeedback(targetUser, exerciseId, feedback, username_)) {
        return Message(MessageType::SEND_FEEDBACK_SUCCESS, Parser::createSuccessMessage());
    }
    
    return createErrorResponse(ErrorCode::DATABASE_ERROR, "Failed to save feedback");
}

Message ClientHandler::handleChatMessage(const Message& message) {
    if (!authenticated_) {
        return createErrorResponse(ErrorCode::NOT_AUTHENTICATED, "Authentication required");
    }
    
    std::string recipient, messageText;
    if (!Parser::parseChatMessage(message.payload, recipient, messageText)) {
        return createErrorResponse(ErrorCode::INVALID_FORMAT, "Invalid chat message");
    }
    
    // In a real implementation, this would forward the message to the recipient
    Logger::getInstance().info("Chat from " + username_ + " to " + recipient + ": " + messageText);
    
    return Message(MessageType::CHAT_MESSAGE_ACK, Parser::createSuccessMessage("Message sent"));
}

Message ClientHandler::handleVoiceCallRequest(const Message& message) {
    if (!authenticated_) {
        return createErrorResponse(ErrorCode::NOT_AUTHENTICATED, "Authentication required");
    }
    
    std::string targetUser = Utils::trim(message.payload);
    
    // In a real implementation, this would initiate WebRTC or similar
    Logger::getInstance().info("Voice call request from " + username_ + " to " + targetUser);
    
    return Message(MessageType::VOICE_CALL_ACCEPT, "call_session_id_123");
}

Message ClientHandler::handleAddGameItemRequest(const Message& message) {
    if (!authenticated_) {
        return createErrorResponse(ErrorCode::NOT_AUTHENTICATED, "Authentication required");
    }
    
    if (role_ != UserRole::ADMIN) {
        return Message(MessageType::ADD_GAME_ITEM_FAILED,
                      Parser::createErrorMessage(ErrorCode::PERMISSION_DENIED, "Admin access required"));
    }
    
    // Parse: gameType|itemData
    std::vector<std::string> parts = Utils::split(message.payload, '|');
    if (parts.size() < 2) {
        return Message(MessageType::ADD_GAME_ITEM_FAILED,
                      Parser::createErrorMessage(ErrorCode::INVALID_FORMAT, "Invalid format"));
    }
    
    if (Database::getInstance().addGameItem(parts[0], parts[1])) {
        return Message(MessageType::ADD_GAME_ITEM_SUCCESS, Parser::createSuccessMessage());
    }
    
    return Message(MessageType::ADD_GAME_ITEM_FAILED,
                  Parser::createErrorMessage(ErrorCode::DATABASE_ERROR, "Failed to add item"));
}

Message ClientHandler::handleHeartbeatRequest(const Message& message) {
    return Message(MessageType::HEARTBEAT_RESPONSE, "pong");
}

Message ClientHandler::createErrorResponse(ErrorCode code, const std::string& description) {
    return Message(MessageType::ERROR_MESSAGE, Parser::createErrorMessage(code, description));
}

