#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "../../include/common.hpp"

// Log levels
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3
};

// Thread-safe logger class for application logging
class Logger {
public:
    // Get singleton instance
    static Logger& getInstance();
    
    // Initialize logger with file path and log level
    void initialize(const std::string& logFile, LogLevel level = LogLevel::INFO);
    
    // Log methods
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    
    // Set log level
    void setLogLevel(LogLevel level);
    
    // Disable copy and assignment
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() : logLevel_(LogLevel::INFO), initialized_(false) {}
    ~Logger();
    
    void log(LogLevel level, const std::string& message);
    std::string levelToString(LogLevel level);
    
    std::string logFilePath_;
    LogLevel logLevel_;
    bool initialized_;
    std::mutex mutex_;
    std::ofstream logFile_;
};

#endif // LOGGER_HPP

