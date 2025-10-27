#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "../../include/common.hpp"
#include "../../include/message_structs.hpp"
#include "../utils/Logger.hpp"

// Simple in-memory database for user management
// In production, this would be replaced with SQLite or other DB
class Database {
public:
    // Get singleton instance
    static Database& getInstance();
    
    // Initialize database
    bool initialize(const std::string& dbFile = "");
    
    // User management
    bool createUser(const std::string& username, const std::string& passwordHash, UserRole role);
    bool authenticateUser(const std::string& username, const std::string& passwordHash);
    bool getUserData(const std::string& username, UserData& userData);
    bool updateUserLevel(const std::string& username, ProficiencyLevel level);
    bool updateUserScore(const std::string& username, int score);
    bool userExists(const std::string& username);
    std::string hashPassword(const std::string& password);
    
    // Session management (in-memory)
    bool createSession(const std::string& username, SOCKET socket);
    bool removeSession(const std::string& username);
    bool getSessionByUsername(const std::string& username, SessionData& session);
    bool getSessionBySocket(SOCKET socket, SessionData& session);
    std::vector<std::string> getOnlineUsers();
    
    // Lesson and content management
    std::vector<std::string> getLessonList(ProficiencyLevel level);
    std::string getLessonContent(const std::string& lessonId);
    
    // Score and feedback management
    bool saveScore(const std::string& username, const std::string& exerciseId, int score);
    bool saveFeedback(const std::string& username, const std::string& exerciseId, 
                     const std::string& feedback, const std::string& fromUser);
    std::vector<std::string> getFeedback(const std::string& username);
    
    // Game content management
    bool addGameItem(const std::string& gameType, const std::string& itemData);
    std::vector<std::string> getGameItems(const std::string& gameType);
    
    // Cleanup
    void clearSessions();
    
    // Disable copy and assignment
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

private:
    Database() : initialized_(false) {}
    ~Database() = default;
    
    bool loadFromFile(const std::string& dbFile);
    bool saveToFile(const std::string& dbFile);
    
    std::map<std::string, UserData> users_;           // username -> user data
    std::map<std::string, SessionData> sessions_;      // username -> session
    std::map<SOCKET, std::string> socketToUser_;       // socket -> username
    std::map<std::string, std::vector<std::string>> lessons_;  // level -> lesson list
    std::map<std::string, std::vector<std::string>> gameItems_; // game type -> items
    std::map<std::string, std::vector<std::string>> feedbacks_; // username -> feedbacks
    
    std::string dbFilePath_;
    bool initialized_;
    std::mutex dbMutex_;
};

#endif // DATABASE_HPP

