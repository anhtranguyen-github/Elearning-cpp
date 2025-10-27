#include "Server.hpp"
#include "../utils/Parser.hpp"
#include <csignal>
#include <iostream>
#include <cstdlib>

// Global server pointer for signal handling
Server* g_server = nullptr;

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\nReceived shutdown signal, stopping server..." << std::endl;
        if (g_server) {
            g_server->stop();
        }
    }
}

int main(int argc, char* argv[]) {
    // Print startup message immediately
    std::cout << "=== English Learning Platform Server ===" << std::endl;
    std::cout << "Initializing..." << std::endl;
    std::cout.flush();
    
    // Initialize logger (create directory if needed)
    std::cout << "Setting up logging..." << std::endl;
    std::cout.flush();
    
    [[maybe_unused]] int ret = system("mkdir -p logs 2>/dev/null");
    Logger::getInstance().initialize("logs/server.log", LogLevel::DEBUG);
    Logger::getInstance().info("=== Server Starting ===");
    
    std::cout << "Loading configuration..." << std::endl;
    std::cout.flush();
    
    // Parse configuration
    std::map<std::string, std::string> config;
    if (!Parser::parseConfigFile("config/server_config.json", config)) {
        Logger::getInstance().warning("Failed to load config, using defaults");
        std::cout << "Using default configuration" << std::endl;
    }
    
    std::string host = config.count("host") ? config["host"] : "0.0.0.0";
    int port = config.count("port") ? std::stoi(config["port"]) : AppConstants::DEFAULT_PORT;
    
    // Override with command line arguments if provided
    if (argc > 1) {
        host = argv[1];
    }
    if (argc > 2) {
        port = std::stoi(argv[2]);
    }
    
    // Initialize database
    std::cout << "Initializing database..." << std::endl;
    std::cout.flush();
    Database::getInstance().initialize("data/users.db");
    
    // Create and initialize server
    std::cout << "Creating server instance..." << std::endl;
    std::cout.flush();
    Server server;
    g_server = &server;
    
    // Register signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    std::cout << "Initializing server on " << host << ":" << port << "..." << std::endl;
    std::cout.flush();
    
    if (!server.initialize(host, port)) {
        Logger::getInstance().error("Failed to initialize server");
        std::cerr << "ERROR: Failed to initialize server!" << std::endl;
        std::cerr << "Check logs/server.log for details" << std::endl;
        return 1;
    }
    
    std::cout << "\n✓ Server listening on " << host << ":" << port << std::endl;
    std::cout << "✓ Database initialized with default accounts:" << std::endl;
    std::cout << "  - admin / admin123 (Admin)" << std::endl;
    std::cout << "  - teacher1 / teacher123 (Teacher)" << std::endl;
    std::cout << "\nPress Ctrl+C to stop the server\n" << std::endl;
    std::cout.flush();
    
    // Run server main loop
    server.run();
    
    Logger::getInstance().info("=== Server Shutdown Complete ===");
    return 0;
}

