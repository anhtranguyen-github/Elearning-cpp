#include "ConsoleClient.hpp"
#include "../utils/Logger.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    // Initialize logger
    Logger::getInstance().initialize("logs/console_client.log", LogLevel::DEBUG);
    Logger::getInstance().info("=== Console Client Starting ===");
    
    try {
        ConsoleClient client;
        client.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        Logger::getInstance().error(std::string("Exception: ") + e.what());
        return 1;
    }
    
    Logger::getInstance().info("=== Console Client Shutdown ===");
    return 0;
}

