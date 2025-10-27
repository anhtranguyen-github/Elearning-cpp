#ifndef PARSER_HPP
#define PARSER_HPP

#include "../../include/common.hpp"
#include "../../include/message_structs.hpp"

// Parser utility class for configuration and data parsing
class Parser {
public:
    // Parse JSON configuration file
    static bool parseConfigFile(const std::string& filePath, 
                                std::map<std::string, std::string>& config);
    
    // Parse message payload for specific message types
    static bool parseLoginRequest(const std::string& payload, 
                                  std::string& username, std::string& password);
    
    static bool parseRegisterRequest(const std::string& payload,
                                     std::string& username, std::string& password, UserRole& role);
    
    static bool parseSetLevelRequest(const std::string& payload, ProficiencyLevel& level);
    
    static bool parseChatMessage(const std::string& payload,
                                std::string& recipient, std::string& message);
    
    // Create message payloads
    static std::string createLoginRequest(const std::string& username, const std::string& password);
    
    static std::string createRegisterRequest(const std::string& username, 
                                             const std::string& password, UserRole role);
    
    static std::string createSetLevelRequest(ProficiencyLevel level);
    
    static std::string createChatMessage(const std::string& recipient, const std::string& message);
    
    static std::string createErrorMessage(ErrorCode code, const std::string& description);
    
    static std::string createSuccessMessage(const std::string& data = "");
    
    // Validate input
    static bool validateUsername(const std::string& username);
    static bool validatePassword(const std::string& password);
    
private:
    Parser() = default;
};

#endif // PARSER_HPP

