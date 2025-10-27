#include "Client.hpp"
#include <thread>
#include <sstream>
#include <iomanip>

Client::Client() 
    : socket_(INVALID_SOCKET), serverPort_(0), connected_(false) {
}

Client::~Client() {
    disconnect();
}

bool Client::connect(const std::string& serverAddress, int serverPort) {
    if (connected_) {
        Logger::getInstance().warning("Already connected");
        return false;
    }
    
    if (!Network::initialize()) {
        Logger::getInstance().error("Failed to initialize network");
        return false;
    }
    
    serverAddress_ = serverAddress;
    serverPort_ = serverPort;
    
    socket_ = Network::createSocket();
    if (!Network::isValidSocket(socket_)) {
        return false;
    }
    
    if (!Network::connectToServer(socket_, serverAddress_, serverPort_)) {
        Network::closeSocket(socket_);
        socket_ = INVALID_SOCKET;
        return false;
    }
    
    // Set socket to non-blocking for receiving
    Network::setNonBlocking(socket_);
    
    connected_ = true;
    Logger::getInstance().info("Connected to server " + serverAddress_ + ":" + std::to_string(serverPort_));
    
    return true;
}

void Client::disconnect() {
    if (!connected_) return;
    
    connected_ = false;
    
    if (Network::isValidSocket(socket_)) {
        Network::closeSocket(socket_);
        socket_ = INVALID_SOCKET;
    }
    
    Network::cleanup();
    Logger::getInstance().info("Disconnected from server");
}

Message Client::sendMessageSync(const Message& message) {
    if (!connected_) {
        return Message(MessageType::ERROR_MESSAGE, 
                      Parser::createErrorMessage(ErrorCode::INTERNAL_ERROR, "Not connected"));
    }
    
    std::lock_guard<std::mutex> lock(socketMutex_);
    
    // Send message
    std::string data = protocol_.encodeMessage(message);
    int bytesSent = Network::sendData(socket_, data.c_str(), data.length());
    
    if (bytesSent <= 0) {
        Logger::getInstance().error("Failed to send message");
        return Message(MessageType::ERROR_MESSAGE, 
                      Parser::createErrorMessage(ErrorCode::INTERNAL_ERROR, "Send failed"));
    }
    
    Logger::getInstance().debug("Sent message type " + std::to_string(static_cast<int>(message.header.type)));
    
    // Wait for response with timeout
    auto startTime = std::chrono::steady_clock::now();
    const int timeoutSeconds = 10;
    
    while (true) {
        receiveData();
        
        // Log buffer contents
        std::ostringstream bufferHex;
        for (unsigned char c : receiveBuffer_) {
            bufferHex << std::hex << std::setw(2) << std::setfill('0') << (int)c << " ";
        }
        Logger::getInstance().debug("CLIENT receiveBuffer_ size=" + std::to_string(receiveBuffer_.size()) + 
                                   " hex=[" + bufferHex.str() + "]");
        
        std::vector<Message> messages = protocol_.extractMessages(receiveBuffer_);
        Logger::getInstance().debug("CLIENT extracted " + std::to_string(messages.size()) + " messages, " +
                                   "remaining buffer size=" + std::to_string(receiveBuffer_.size()));
        
        if (!messages.empty()) {
            Logger::getInstance().debug("CLIENT returning message type " + std::to_string((int)messages[0].header.type));
            return messages[0]; // Return first message
        }
        
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - startTime).count();
        
        if (elapsed > timeoutSeconds) {
            Logger::getInstance().error("Response timeout");
            return Message(MessageType::ERROR_MESSAGE, 
                          Parser::createErrorMessage(ErrorCode::INTERNAL_ERROR, "Timeout"));
        }
        
        // Small delay to avoid busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

bool Client::sendMessageAsync(const Message& message) {
    if (!connected_) return false;
    
    std::lock_guard<std::mutex> lock(socketMutex_);
    
    std::string data = protocol_.encodeMessage(message);
    int bytesSent = Network::sendData(socket_, data.c_str(), data.length());
    
    return bytesSent > 0;
}

bool Client::receiveMessage(Message& message) {
    if (!receiveData()) {
        return false;
    }
    
    std::vector<Message> messages = protocol_.extractMessages(receiveBuffer_);
    if (messages.empty()) {
        return false;
    }
    
    message = messages[0];
    return true;
}

std::vector<Message> Client::pollMessages() {
    receiveData();
    return protocol_.extractMessages(receiveBuffer_);
}

bool Client::receiveData() {
    if (!connected_) return false;
    
    char buffer[AppConstants::BUFFER_SIZE];
    int bytesReceived = Network::receiveData(socket_, buffer, sizeof(buffer) - 1);
    
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        receiveBuffer_ += std::string(buffer, bytesReceived);
        return true;
    }
    
    return false;
}

bool Client::registerUser(const std::string& username, const std::string& password, UserRole role) {
    std::string payload = Parser::createRegisterRequest(username, password, role);
    Message request(MessageType::REGISTER_REQUEST, payload);
    
    Message response = sendMessageSync(request);
    
    if (response.header.type == MessageType::REGISTER_SUCCESS) {
        Logger::getInstance().info("User registered successfully: " + username);
        return true;
    }
    
    Logger::getInstance().error("Registration failed: " + response.payload);
    return false;
}

bool Client::login(const std::string& username, const std::string& password, UserData& userData) {
    std::string payload = Parser::createLoginRequest(username, password);
    Message request(MessageType::LOGIN_REQUEST, payload);
    
    Message response = sendMessageSync(request);
    
    if (response.header.type == MessageType::LOGIN_SUCCESS) {
        // Parse response: role|level|score
        std::vector<std::string> parts = Utils::split(response.payload, '|');
        if (parts.size() >= 3) {
            userData.username = username;
            userData.role = static_cast<UserRole>(std::stoi(parts[0]));
            userData.level = static_cast<ProficiencyLevel>(std::stoi(parts[1]));
            userData.score = std::stoi(parts[2]);
        }
        
        Logger::getInstance().info("Logged in successfully: " + username);
        return true;
    }
    
    Logger::getInstance().error("Login failed: " + response.payload);
    return false;
}

bool Client::logout() {
    Message request(MessageType::LOGOUT_REQUEST);
    Message response = sendMessageSync(request);
    
    if (response.header.type == MessageType::LOGOUT_SUCCESS) {
        Logger::getInstance().info("Logged out successfully");
        return true;
    }
    
    return false;
}

bool Client::setLevel(ProficiencyLevel level) {
    std::string payload = Parser::createSetLevelRequest(level);
    Message request(MessageType::SET_LEVEL_REQUEST, payload);
    
    Message response = sendMessageSync(request);
    return response.header.type == MessageType::SET_LEVEL_SUCCESS;
}

std::vector<std::string> Client::getLessonList() {
    Message request(MessageType::GET_LESSON_LIST_REQUEST);
    Message response = sendMessageSync(request);
    
    if (response.header.type == MessageType::GET_LESSON_LIST_RESPONSE) {
        return Utils::split(response.payload, ';');
    }
    
    return {};
}

std::string Client::getLessonContent(const std::string& lessonId) {
    Message request(MessageType::GET_LESSON_CONTENT_REQUEST, lessonId);
    Message response = sendMessageSync(request);
    
    if (response.header.type == MessageType::GET_LESSON_CONTENT_RESPONSE) {
        return response.payload;
    }
    
    return "";
}

bool Client::submitQuiz(const std::string& quizId, const std::string& answers, int& score) {
    std::string payload = quizId + "|" + answers;
    Message request(MessageType::SUBMIT_QUIZ_REQUEST, payload);
    
    Message response = sendMessageSync(request);
    
    if (response.header.type == MessageType::SUBMIT_QUIZ_RESPONSE) {
        std::vector<std::string> parts = Utils::split(response.payload, '|');
        if (!parts.empty()) {
            score = std::stoi(parts[0]);
        }
        return true;
    }
    
    return false;
}

bool Client::submitExercise(const std::string& exerciseId, const std::string& content, int& score) {
    std::string payload = exerciseId + "|" + content;
    Message request(MessageType::SUBMIT_EXERCISE_REQUEST, payload);
    
    Message response = sendMessageSync(request);
    
    if (response.header.type == MessageType::SUBMIT_EXERCISE_RESPONSE) {
        score = std::stoi(response.payload);
        return true;
    }
    
    return false;
}

std::string Client::startGame(const std::string& gameType) {
    Message request(MessageType::GAME_START_REQUEST, gameType);
    Message response = sendMessageSync(request);
    
    if (response.header.type == MessageType::GAME_START_RESPONSE) {
        return response.payload;
    }
    
    return "";
}

bool Client::sendGameMove(const std::string& moveData, std::string& response) {
    Message request(MessageType::GAME_MOVE_REQUEST, moveData);
    Message responseMsg = sendMessageSync(request);
    
    if (responseMsg.header.type == MessageType::GAME_MOVE_RESPONSE) {
        response = responseMsg.payload;
        return true;
    }
    
    return false;
}

bool Client::sendChatMessage(const std::string& recipient, const std::string& message) {
    std::string payload = Parser::createChatMessage(recipient, message);
    Message request(MessageType::CHAT_MESSAGE, payload);
    
    Message response = sendMessageSync(request);
    return response.header.type == MessageType::CHAT_MESSAGE_ACK;
}

bool Client::initiateVoiceCall(const std::string& targetUser) {
    Message request(MessageType::VOICE_CALL_REQUEST, targetUser);
    Message response = sendMessageSync(request);
    
    return response.header.type == MessageType::VOICE_CALL_ACCEPT;
}

int Client::getScore() {
    Message request(MessageType::GET_SCORE_REQUEST);
    Message response = sendMessageSync(request);
    
    if (response.header.type == MessageType::GET_SCORE_RESPONSE) {
        return std::stoi(response.payload);
    }
    
    return 0;
}

std::vector<std::string> Client::getFeedback() {
    Message request(MessageType::GET_FEEDBACK_REQUEST);
    Message response = sendMessageSync(request);
    
    if (response.header.type == MessageType::GET_FEEDBACK_RESPONSE) {
        return Utils::split(response.payload, ';');
    }
    
    return {};
}

bool Client::sendHeartbeat() {
    Message request(MessageType::HEARTBEAT_REQUEST, "ping");
    Message response = sendMessageSync(request);
    
    return response.header.type == MessageType::HEARTBEAT_RESPONSE;
}

