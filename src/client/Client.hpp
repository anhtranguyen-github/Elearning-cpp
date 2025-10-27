#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../../include/common.hpp"
#include "../../include/message_structs.hpp"
#include "../protocol/Protocol.hpp"
#include "../protocol/Network.hpp"
#include "../utils/Logger.hpp"
#include "../utils/Parser.hpp"

// Client class for connecting to server and handling communication
class Client {
public:
    Client();
    ~Client();
    
    // Connect to server
    bool connect(const std::string& serverAddress, int serverPort);
    
    // Disconnect from server
    void disconnect();
    
    // Check if connected
    bool isConnected() const { return connected_; }
    
    // Send message and wait for response
    Message sendMessageSync(const Message& message);
    
    // Send message asynchronously
    bool sendMessageAsync(const Message& message);
    
    // Receive message (non-blocking)
    bool receiveMessage(Message& message);
    
    // Poll for incoming messages
    std::vector<Message> pollMessages();
    
    // User operations
    bool registerUser(const std::string& username, const std::string& password, UserRole role);
    bool login(const std::string& username, const std::string& password, UserData& userData);
    bool logout();
    
    // Study operations
    bool setLevel(ProficiencyLevel level);
    std::vector<std::string> getLessonList();
    std::string getLessonContent(const std::string& lessonId);
    
    // Exercise operations
    bool submitQuiz(const std::string& quizId, const std::string& answers, int& score);
    bool submitExercise(const std::string& exerciseId, const std::string& content, int& score);
    
    // Game operations
    std::string startGame(const std::string& gameType);
    bool sendGameMove(const std::string& moveData, std::string& response);
    
    // Communication operations
    bool sendChatMessage(const std::string& recipient, const std::string& message);
    bool initiateVoiceCall(const std::string& targetUser);
    
    // Score and feedback
    int getScore();
    std::vector<std::string> getFeedback();
    
    // Heartbeat
    bool sendHeartbeat();

private:
    bool receiveData();
    
    SOCKET socket_;
    std::string serverAddress_;
    int serverPort_;
    bool connected_;
    
    std::string receiveBuffer_;
    Protocol protocol_;
    
    std::mutex socketMutex_;
};

#endif // CLIENT_HPP

