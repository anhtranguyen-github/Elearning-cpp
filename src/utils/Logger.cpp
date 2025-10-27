#include "Logger.hpp"

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::initialize(const std::string& logFile, LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (initialized_) {
        if (logFile_.is_open()) {
            logFile_.close();
        }
    }
    
    logFilePath_ = logFile;
    logLevel_ = level;
    
    logFile_.open(logFilePath_, std::ios::app);
    if (!logFile_.is_open()) {
        std::cerr << "Failed to open log file: " << logFilePath_ << std::endl;
        initialized_ = false;
        return;
    }
    
    initialized_ = true;
    // Write initial log entry directly (avoid deadlock by not calling log())
    if (logFile_.is_open()) {
        std::string logEntry = Utils::getCurrentTimestamp() + " [INFO] Logger initialized";
        logFile_ << logEntry << std::endl;
        logFile_.flush();
    }
}

Logger::~Logger() {
    if (logFile_.is_open()) {
        // Write shutdown message directly (avoid potential deadlock)
        std::string logEntry = Utils::getCurrentTimestamp() + " [INFO] Logger shutting down";
        logFile_ << logEntry << std::endl;
        logFile_.close();
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    if (!initialized_ || level < logLevel_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string logEntry = Utils::getCurrentTimestamp() + " [" + 
                           levelToString(level) + "] " + message;
    
    // Write to file
    if (logFile_.is_open()) {
        logFile_ << logEntry << std::endl;
        logFile_.flush();
    }
    
    // Also output to console for errors
    if (level == LogLevel::ERROR) {
        std::cerr << logEntry << std::endl;
    }
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    logLevel_ = level;
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

