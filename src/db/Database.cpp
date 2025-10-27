#include "Database.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

Database& Database::getInstance() {
    static Database instance;
    return instance;
}

bool Database::initialize(const std::string& dbFile) {
    // No lock needed here - this is only called once at startup before any concurrent access
    dbFilePath_ = dbFile;
    
    // Initialize sample lessons for each level
    lessons_["BEGINNER"] = {
        "lesson_b1:Greetings and Introductions",
        "lesson_b2:Numbers and Time",
        "lesson_b3:Family and Friends"
    };
    
    lessons_["INTERMEDIATE"] = {
        "lesson_i1:Travel and Transportation",
        "lesson_i2:Food and Cooking",
        "lesson_i3:Work and Career"
    };
    
    lessons_["ADVANCED"] = {
        "lesson_a1:Business Communication",
        "lesson_a2:Academic Writing",
        "lesson_a3:Cultural Studies"
    };
    
    // Create default admin user (these call createUser which has its own locking)
    createUser("admin", hashPassword("admin123"), UserRole::ADMIN);
    createUser("teacher1", hashPassword("teacher123"), UserRole::TEACHER);
    
    initialized_ = true;
    Logger::getInstance().info("Database initialized");
    
    return true;
}

bool Database::createUser(const std::string& username, const std::string& passwordHash, UserRole role) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    if (users_.find(username) != users_.end()) {
        return false; // User already exists
    }
    
    UserData user;
    user.username = username;
    user.passwordHash = passwordHash;
    user.role = role;
    user.level = ProficiencyLevel::BEGINNER;
    user.score = 0;
    
    users_[username] = user;
    
    Logger::getInstance().info("User created: " + username + " with role " + std::to_string(static_cast<int>(role)));
    return true;
}

bool Database::authenticateUser(const std::string& username, const std::string& passwordHash) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    auto it = users_.find(username);
    if (it == users_.end()) {
        return false;
    }
    
    return it->second.passwordHash == passwordHash;
}

bool Database::getUserData(const std::string& username, UserData& userData) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    auto it = users_.find(username);
    if (it == users_.end()) {
        return false;
    }
    
    userData = it->second;
    return true;
}

bool Database::updateUserLevel(const std::string& username, ProficiencyLevel level) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    auto it = users_.find(username);
    if (it == users_.end()) {
        return false;
    }
    
    it->second.level = level;
    Logger::getInstance().info("User " + username + " level updated to " + std::to_string(static_cast<int>(level)));
    return true;
}

bool Database::updateUserScore(const std::string& username, int score) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    auto it = users_.find(username);
    if (it == users_.end()) {
        return false;
    }
    
    it->second.score += score;
    Logger::getInstance().info("User " + username + " score updated: +" + std::to_string(score));
    return true;
}

bool Database::userExists(const std::string& username) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    return users_.find(username) != users_.end();
}

bool Database::createSession(const std::string& username, SOCKET socket) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    SessionData session;
    session.socket = socket;
    session.username = username;
    session.state = ConnectionState::AUTHENTICATED;
    session.lastActivity = std::chrono::steady_clock::now();
    
    auto userIt = users_.find(username);
    if (userIt != users_.end()) {
        session.role = userIt->second.role;
    }
    
    sessions_[username] = session;
    socketToUser_[socket] = username;
    
    Logger::getInstance().info("Session created for user: " + username);
    return true;
}

bool Database::removeSession(const std::string& username) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    auto it = sessions_.find(username);
    if (it == sessions_.end()) {
        return false;
    }
    
    SOCKET socket = it->second.socket;
    sessions_.erase(it);
    socketToUser_.erase(socket);
    
    Logger::getInstance().info("Session removed for user: " + username);
    return true;
}

bool Database::getSessionByUsername(const std::string& username, SessionData& session) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    auto it = sessions_.find(username);
    if (it == sessions_.end()) {
        return false;
    }
    
    session = it->second;
    return true;
}

bool Database::getSessionBySocket(SOCKET socket, SessionData& session) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    auto it = socketToUser_.find(socket);
    if (it == socketToUser_.end()) {
        return false;
    }
    
    return getSessionByUsername(it->second, session);
}

std::vector<std::string> Database::getOnlineUsers() {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    std::vector<std::string> users;
    for (const auto& pair : sessions_) {
        users.push_back(pair.first);
    }
    
    return users;
}

std::vector<std::string> Database::getLessonList(ProficiencyLevel level) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    std::string levelStr;
    switch (level) {
        case ProficiencyLevel::BEGINNER: levelStr = "BEGINNER"; break;
        case ProficiencyLevel::INTERMEDIATE: levelStr = "INTERMEDIATE"; break;
        case ProficiencyLevel::ADVANCED: levelStr = "ADVANCED"; break;
    }
    
    auto it = lessons_.find(levelStr);
    if (it == lessons_.end()) {
        return {};
    }
    
    return it->second;
}

std::string Database::getLessonContent(const std::string& lessonId) {
    // In a real implementation, this would fetch from a content database
    return "Content for lesson: " + lessonId + "\nVideo: video_url\nAudio: audio_url\nText: lesson_text";
}

bool Database::saveScore(const std::string& username, const std::string& exerciseId, int score) {
    Logger::getInstance().info("Score saved for " + username + " on " + exerciseId + ": " + std::to_string(score));
    return updateUserScore(username, score);
}

bool Database::saveFeedback(const std::string& username, const std::string& exerciseId, 
                           const std::string& feedback, const std::string& fromUser) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    std::string feedbackEntry = "Exercise: " + exerciseId + " | From: " + fromUser + " | " + feedback;
    feedbacks_[username].push_back(feedbackEntry);
    
    Logger::getInstance().info("Feedback saved for " + username + " from " + fromUser);
    return true;
}

std::vector<std::string> Database::getFeedback(const std::string& username) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    auto it = feedbacks_.find(username);
    if (it == feedbacks_.end()) {
        return {};
    }
    
    return it->second;
}

bool Database::addGameItem(const std::string& gameType, const std::string& itemData) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    gameItems_[gameType].push_back(itemData);
    Logger::getInstance().info("Game item added to " + gameType);
    return true;
}

std::vector<std::string> Database::getGameItems(const std::string& gameType) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    auto it = gameItems_.find(gameType);
    if (it == gameItems_.end()) {
        return {};
    }
    
    return it->second;
}

void Database::clearSessions() {
    std::lock_guard<std::mutex> lock(dbMutex_);
    
    sessions_.clear();
    socketToUser_.clear();
    Logger::getInstance().info("All sessions cleared");
}

std::string Database::hashPassword(const std::string& password) {
    // Simple hash function (in production, use proper crypto like bcrypt/scrypt)
    std::hash<std::string> hasher;
    size_t hash = hasher(password + "salt_value");
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << hash;
    return ss.str();
}

