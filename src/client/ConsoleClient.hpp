#ifndef CONSOLE_CLIENT_HPP
#define CONSOLE_CLIENT_HPP

#include "../../include/common.hpp"
#include "../../include/message_structs.hpp"
#include "Client.hpp"
#include <string>
#include <map>
#include <functional>

// Console-based client with menu-driven interface
class ConsoleClient {
public:
    ConsoleClient();
    ~ConsoleClient();
    
    // Main application loop
    void run();

private:
    // Connection management
    void connectToServer();
    void disconnect();
    
    // Authentication
    void registerUser();
    void login();
    void logout();
    
    // Student features
    void studentMenu();
    void setLevel();
    void browseLessons();
    void viewLesson();
    void submitQuiz();
    void submitExercise();
    void playGame();
    void chat();
    void viewScoreAndFeedback();
    
    // Teacher features
    void teacherMenu();
    void provideFeedback();
    void chatWithStudent();
    
    // Admin features
    void adminMenu();
    void addGameContent();
    
    // UI helpers
    void clearScreen();
    void printHeader(const std::string& title);
    void printMenu(const std::vector<std::string>& options);
    int getChoice(int max);
    std::string getInput(const std::string& prompt);
    std::string getPassword(const std::string& prompt);
    void pause();
    void printMessage(const std::string& message, bool isError = false);
    void printSuccess(const std::string& message);
    void printError(const std::string& message);
    
    // State
    std::unique_ptr<Client> client_;
    bool running_;
    bool connected_;
    bool authenticated_;
    UserData currentUser_;
};

#endif // CONSOLE_CLIENT_HPP

