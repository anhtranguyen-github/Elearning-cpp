#include "UI.hpp"
#include "../utils/Logger.hpp"

#include <QApplication>
#include <iostream>

int main(int argc, char* argv[]) {
    // Initialize logger
    Logger::getInstance().initialize("logs/client.log", LogLevel::INFO);
    Logger::getInstance().info("=== Client Starting ===");
    
    // Create Qt application
    QApplication app(argc, argv);
    
    // Create and show main window
    MainWindow window;
    window.show();
    
    Logger::getInstance().info("Client UI initialized");
    
    // Run application event loop
    int result = app.exec();
    
    Logger::getInstance().info("=== Client Shutdown Complete ===");
    return result;
}

