#include "Parser.hpp"
#include <fstream>
#include <sstream>

bool Parser::parseConfigFile(const std::string& filePath, 
                             std::map<std::string, std::string>& config) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    // Simple JSON-like parsing (without external library dependency for basic config)
    // For production, use a proper JSON library like nlohmann/json or jsoncpp
    // Here we'll do simple key:value parsing
    
    size_t pos = 0;
    while ((pos = content.find("\"", pos)) != std::string::npos) {
        size_t keyStart = pos + 1;
        size_t keyEnd = content.find("\"", keyStart);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyStart, keyEnd - keyStart);
        
        pos = content.find(":", keyEnd);
        if (pos == std::string::npos) break;
        
        pos++;
        while (pos < content.length() && (content[pos] == ' ' || content[pos] == '\t')) pos++;
        
        if (pos >= content.length()) break;
        
        std::string value;
        if (content[pos] == '"') {
            size_t valueStart = pos + 1;
            size_t valueEnd = content.find("\"", valueStart);
            if (valueEnd == std::string::npos) break;
            value = content.substr(valueStart, valueEnd - valueStart);
            pos = valueEnd + 1;
        } else {
            size_t valueStart = pos;
            size_t valueEnd = content.find_first_of(",}\n", valueStart);
            if (valueEnd == std::string::npos) valueEnd = content.length();
            value = Utils::trim(content.substr(valueStart, valueEnd - valueStart));
            pos = valueEnd;
        }
        
        config[key] = value;
    }
    
    return !config.empty();
}

bool Parser::parseLoginRequest(const std::string& payload, 
                               std::string& username, std::string& password) {
    std::vector<std::string> parts = Utils::split(payload, '|');
    if (parts.size() < 2) return false;
    
    username = Utils::trim(parts[0]);
    password = Utils::trim(parts[1]);
    
    return validateUsername(username) && validatePassword(password);
}

bool Parser::parseRegisterRequest(const std::string& payload,
                                  std::string& username, std::string& password, UserRole& role) {
    std::vector<std::string> parts = Utils::split(payload, '|');
    if (parts.size() < 3) return false;
    
    username = Utils::trim(parts[0]);
    password = Utils::trim(parts[1]);
    
    try {
        role = static_cast<UserRole>(std::stoi(parts[2]));
    } catch (...) {
        return false;
    }
    
    return validateUsername(username) && validatePassword(password);
}

bool Parser::parseSetLevelRequest(const std::string& payload, ProficiencyLevel& level) {
    try {
        int levelValue = std::stoi(Utils::trim(payload));
        if (levelValue < 1 || levelValue > 3) return false;
        level = static_cast<ProficiencyLevel>(levelValue);
        return true;
    } catch (...) {
        return false;
    }
}

bool Parser::parseChatMessage(const std::string& payload,
                             std::string& recipient, std::string& message) {
    size_t pos = payload.find('|');
    if (pos == std::string::npos) return false;
    
    recipient = Utils::trim(payload.substr(0, pos));
    message = payload.substr(pos + 1);
    
    return !recipient.empty() && !message.empty();
}

std::string Parser::createLoginRequest(const std::string& username, const std::string& password) {
    return username + "|" + password;
}

std::string Parser::createRegisterRequest(const std::string& username, 
                                         const std::string& password, UserRole role) {
    return username + "|" + password + "|" + std::to_string(static_cast<int>(role));
}

std::string Parser::createSetLevelRequest(ProficiencyLevel level) {
    return std::to_string(static_cast<int>(level));
}

std::string Parser::createChatMessage(const std::string& recipient, const std::string& message) {
    return recipient + "|" + message;
}

std::string Parser::createErrorMessage(ErrorCode code, const std::string& description) {
    return std::to_string(static_cast<int>(code)) + "|" + description;
}

std::string Parser::createSuccessMessage(const std::string& data) {
    return std::to_string(static_cast<int>(ErrorCode::SUCCESS)) + 
           (data.empty() ? "" : "|" + data);
}

bool Parser::validateUsername(const std::string& username) {
    if (username.empty() || username.length() > 50) return false;
    
    // Username should contain only alphanumeric characters and underscores
    for (char c : username) {
        if (!std::isalnum(c) && c != '_') return false;
    }
    
    return true;
}

bool Parser::validatePassword(const std::string& password) {
    // Password should be at least 4 characters (simple validation)
    return password.length() >= 4 && password.length() <= 100;
}

