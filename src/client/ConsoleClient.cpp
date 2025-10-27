#include "ConsoleClient.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

#ifdef _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

ConsoleClient::ConsoleClient() 
    : running_(true), connected_(false), authenticated_(false) {
    client_ = std::make_unique<Client>();
}

ConsoleClient::~ConsoleClient() {
    if (connected_) {
        client_->disconnect();
    }
}

void ConsoleClient::run() {
    clearScreen();
    printHeader("English Learning Platform - Console Client");
    
    std::cout << "\nWelcome to the English Learning Platform!\n" << std::endl;
    
    // Main menu loop
    while (running_) {
        if (!connected_) {
            // Not connected menu
            printMenu({
                "Connect to Server",
                "Exit"
            });
            
            int choice = getChoice(2);
            switch (choice) {
                case 1:
                    connectToServer();
                    break;
                case 2:
                    running_ = false;
                    std::cout << "\nGoodbye!\n" << std::endl;
                    break;
            }
        }
        else if (!authenticated_) {
            // Connected but not authenticated
            printMenu({
                "Login",
                "Register New Account",
                "Disconnect"
            });
            
            int choice = getChoice(3);
            switch (choice) {
                case 1:
                    login();
                    break;
                case 2:
                    registerUser();
                    break;
                case 3:
                    disconnect();
                    break;
            }
        }
        else {
            // Authenticated - show role-specific menu
            switch (currentUser_.role) {
                case UserRole::STUDENT:
                    studentMenu();
                    break;
                case UserRole::TEACHER:
                    teacherMenu();
                    break;
                case UserRole::ADMIN:
                    adminMenu();
                    break;
            }
        }
    }
}

// ==================== Connection Management ====================
void ConsoleClient::connectToServer() {
    clearScreen();
    printHeader("Connect to Server");
    
    std::string address = getInput("Server address [127.0.0.1]: ");
    if (address.empty()) address = "127.0.0.1";
    
    std::string portStr = getInput("Server port [8080]: ");
    int port = portStr.empty() ? 8080 : std::stoi(portStr);
    
    std::cout << "\nConnecting to " << address << ":" << port << "..." << std::endl;
    
    if (client_->connect(address, port)) {
        connected_ = true;
        printSuccess("✓ Connected to server successfully!");
    } else {
        printError("✗ Failed to connect to server");
    }
    
    pause();
}

void ConsoleClient::disconnect() {
    if (authenticated_) {
        client_->logout();
        authenticated_ = false;
    }
    client_->disconnect();
    connected_ = false;
    printSuccess("Disconnected from server");
    pause();
}

// ==================== Authentication ====================
void ConsoleClient::registerUser() {
    clearScreen();
    printHeader("Register New Account");
    
    std::string username = getInput("Choose username: ");
    std::string password = getPassword("Choose password: ");
    std::string confirm = getPassword("Confirm password: ");
    
    if (password != confirm) {
        printError("Passwords do not match!");
        pause();
        return;
    }
    
    // Ask for role
    std::cout << "\nSelect role:" << std::endl;
    std::cout << "  1. Student" << std::endl;
    std::cout << "  2. Teacher" << std::endl;
    std::cout << "  3. Admin" << std::endl;
    
    int roleChoice = getChoice(3);
    UserRole role = static_cast<UserRole>(roleChoice);
    
    std::cout << "\nRegistering account..." << std::endl;
    
    if (client_->registerUser(username, password, role)) {
        printSuccess("✓ Registration successful! You can now login.");
    } else {
        printError("✗ Registration failed. Username may already exist.");
    }
    
    pause();
}

void ConsoleClient::login() {
    clearScreen();
    printHeader("Login");
    
    std::string username = getInput("Username: ");
    std::string password = getPassword("Password: ");
    
    std::cout << "\nAuthenticating..." << std::endl;
    
    UserData userData;
    if (client_->login(username, password, userData)) {
        authenticated_ = true;
        currentUser_ = userData;
        
        clearScreen();
        printSuccess("✓ Login successful!");
        std::cout << "\nWelcome, " << username << "!" << std::endl;
        std::cout << "Role: ";
        switch (userData.role) {
            case UserRole::STUDENT: std::cout << "Student"; break;
            case UserRole::TEACHER: std::cout << "Teacher"; break;
            case UserRole::ADMIN: std::cout << "Admin"; break;
        }
        std::cout << std::endl;
        std::cout << "Level: ";
        switch (userData.level) {
            case ProficiencyLevel::BEGINNER: std::cout << "Beginner"; break;
            case ProficiencyLevel::INTERMEDIATE: std::cout << "Intermediate"; break;
            case ProficiencyLevel::ADVANCED: std::cout << "Advanced"; break;
        }
        std::cout << std::endl;
        std::cout << "Score: " << userData.score << std::endl;
    } else {
        printError("✗ Login failed. Invalid credentials.");
    }
    
    pause();
}

void ConsoleClient::logout() {
    client_->logout();
    authenticated_ = false;
    printSuccess("Logged out successfully");
    pause();
}

// ==================== Student Menu ====================
void ConsoleClient::studentMenu() {
    clearScreen();
    printHeader("Student Dashboard");
    
    std::cout << "Welcome, " << currentUser_.username << "!" << std::endl;
    std::cout << "Level: ";
    switch (currentUser_.level) {
        case ProficiencyLevel::BEGINNER: std::cout << "Beginner"; break;
        case ProficiencyLevel::INTERMEDIATE: std::cout << "Intermediate"; break;
        case ProficiencyLevel::ADVANCED: std::cout << "Advanced"; break;
    }
    std::cout << " | Score: " << currentUser_.score << "\n" << std::endl;
    
    printMenu({
        "Set Proficiency Level",
        "Browse Lessons",
        "Submit Quiz",
        "Submit Exercise",
        "Play Game",
        "Chat",
        "View Score & Feedback",
        "Logout"
    });
    
    int choice = getChoice(8);
    switch (choice) {
        case 1: setLevel(); break;
        case 2: browseLessons(); break;
        case 3: submitQuiz(); break;
        case 4: submitExercise(); break;
        case 5: playGame(); break;
        case 6: chat(); break;
        case 7: viewScoreAndFeedback(); break;
        case 8: logout(); break;
    }
}

void ConsoleClient::setLevel() {
    clearScreen();
    printHeader("Set Proficiency Level");
    
    std::cout << "Current level: ";
    switch (currentUser_.level) {
        case ProficiencyLevel::BEGINNER: std::cout << "Beginner"; break;
        case ProficiencyLevel::INTERMEDIATE: std::cout << "Intermediate"; break;
        case ProficiencyLevel::ADVANCED: std::cout << "Advanced"; break;
    }
    std::cout << "\n" << std::endl;
    
    printMenu({
        "Beginner",
        "Intermediate",
        "Advanced"
    });
    
    int choice = getChoice(3);
    ProficiencyLevel level = static_cast<ProficiencyLevel>(choice);
    
    if (client_->setLevel(level)) {
        currentUser_.level = level;
        printSuccess("✓ Level updated successfully!");
    } else {
        printError("✗ Failed to update level");
    }
    
    pause();
}

void ConsoleClient::browseLessons() {
    clearScreen();
    printHeader("Browse Lessons");
    
    std::cout << "Fetching lessons for your level..." << std::endl;
    
    std::vector<std::string> lessons = client_->getLessonList();
    
    if (lessons.empty()) {
        printError("No lessons available");
        pause();
        return;
    }
    
    std::cout << "\nAvailable Lessons:\n" << std::endl;
    for (size_t i = 0; i < lessons.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << lessons[i] << std::endl;
    }
    
    std::cout << "\n  0. Back" << std::endl;
    
    int choice = getChoice(lessons.size());
    if (choice > 0) {
        // Extract lesson ID
        std::string lessonId = lessons[choice - 1];
        size_t colonPos = lessonId.find(':');
        if (colonPos != std::string::npos) {
            lessonId = lessonId.substr(0, colonPos);
        }
        
        clearScreen();
        printHeader("Lesson: " + lessons[choice - 1]);
        
        std::string content = client_->getLessonContent(lessonId);
        std::cout << "\n" << content << "\n" << std::endl;
        
        pause();
    }
}

void ConsoleClient::submitQuiz() {
    clearScreen();
    printHeader("Submit Quiz");
    
    std::string quizId = getInput("Quiz ID: ");
    std::string answers = getInput("Your answers: ");
    
    std::cout << "\nSubmitting quiz..." << std::endl;
    
    int score = 0;
    if (client_->submitQuiz(quizId, answers, score)) {
        printSuccess("✓ Quiz submitted successfully!");
        std::cout << "Score: +" << score << " points" << std::endl;
        currentUser_.score += score;
    } else {
        printError("✗ Failed to submit quiz");
    }
    
    pause();
}

void ConsoleClient::submitExercise() {
    clearScreen();
    printHeader("Submit Exercise");
    
    std::string exerciseId = getInput("Exercise ID: ");
    std::cout << "\nEnter your answer (press Enter twice to finish):\n" << std::endl;
    
    std::string content, line;
    while (std::getline(std::cin, line)) {
        if (line.empty() && !content.empty()) break;
        content += line + "\n";
    }
    
    std::cout << "Submitting exercise..." << std::endl;
    
    int score = 0;
    if (client_->submitExercise(exerciseId, content, score)) {
        printSuccess("✓ Exercise submitted successfully!");
        std::cout << "Score: +" << score << " points" << std::endl;
        currentUser_.score += score;
    } else {
        printError("✗ Failed to submit exercise");
    }
    
    pause();
}

void ConsoleClient::playGame() {
    clearScreen();
    printHeader("Play Game");
    
    printMenu({
        "Word Matching",
        "Sentence Matching",
        "Picture Matching"
    });
    
    int choice = getChoice(3);
    std::string gameType;
    switch (choice) {
        case 1: gameType = "Word Matching"; break;
        case 2: gameType = "Sentence Matching"; break;
        case 3: gameType = "Picture Matching"; break;
    }
    
    std::cout << "\nStarting " << gameType << "..." << std::endl;
    
    std::string gameData = client_->startGame(gameType);
    if (!gameData.empty()) {
        std::cout << "\nGame started!" << std::endl;
        std::cout << gameData << "\n" << std::endl;
        
        std::string move = getInput("Enter your move: ");
        std::string response;
        
        if (client_->sendGameMove(move, response)) {
            std::cout << "\nResult: " << response << std::endl;
        }
    } else {
        printError("Failed to start game");
    }
    
    pause();
}

void ConsoleClient::chat() {
    clearScreen();
    printHeader("Chat");
    
    std::string recipient = getInput("Recipient username: ");
    std::string message = getInput("Message: ");
    
    if (client_->sendChatMessage(recipient, message)) {
        printSuccess("✓ Message sent to " + recipient);
    } else {
        printError("✗ Failed to send message");
    }
    
    pause();
}

void ConsoleClient::viewScoreAndFeedback() {
    clearScreen();
    printHeader("Score & Feedback");
    
    int score = client_->getScore();
    std::cout << "Total Score: " << score << " points\n" << std::endl;
    
    std::cout << "Fetching feedback..." << std::endl;
    std::vector<std::string> feedbacks = client_->getFeedback();
    
    if (feedbacks.empty()) {
        std::cout << "No feedback available yet." << std::endl;
    } else {
        std::cout << "\nFeedback:\n" << std::endl;
        for (const auto& feedback : feedbacks) {
            std::cout << "  • " << feedback << std::endl;
        }
    }
    
    pause();
}

// ==================== Teacher Menu ====================
void ConsoleClient::teacherMenu() {
    clearScreen();
    printHeader("Teacher Dashboard");
    
    std::cout << "Welcome, Teacher " << currentUser_.username << "!\n" << std::endl;
    
    printMenu({
        "Provide Feedback to Student",
        "Chat with Student",
        "Logout"
    });
    
    int choice = getChoice(3);
    switch (choice) {
        case 1: provideFeedback(); break;
        case 2: chatWithStudent(); break;
        case 3: logout(); break;
    }
}

void ConsoleClient::provideFeedback() {
    clearScreen();
    printHeader("Provide Feedback");
    
    std::string studentName = getInput("Student username: ");
    std::string exerciseId = getInput("Exercise ID: ");
    
    std::cout << "\nEnter feedback (press Enter twice to finish):\n" << std::endl;
    
    std::string feedback, line;
    while (std::getline(std::cin, line)) {
        if (line.empty() && !feedback.empty()) break;
        feedback += line + "\n";
    }
    
    // Send feedback using SEND_FEEDBACK_REQUEST
    std::string payload = studentName + "|" + exerciseId + "|" + feedback;
    Message request(MessageType::SEND_FEEDBACK_REQUEST, payload);
    Message response = client_->sendMessageSync(request);
    
    if (response.header.type == MessageType::SEND_FEEDBACK_SUCCESS) {
        printSuccess("✓ Feedback sent to " + studentName);
    } else {
        printError("✗ Failed to send feedback");
    }
    
    pause();
}

void ConsoleClient::chatWithStudent() {
    clearScreen();
    printHeader("Chat with Student");
    
    std::string student = getInput("Student username: ");
    std::string message = getInput("Message: ");
    
    if (client_->sendChatMessage(student, message)) {
        printSuccess("✓ Message sent to " + student);
    } else {
        printError("✗ Failed to send message");
    }
    
    pause();
}

// ==================== Admin Menu ====================
void ConsoleClient::adminMenu() {
    clearScreen();
    printHeader("Admin Dashboard");
    
    std::cout << "Welcome, Admin " << currentUser_.username << "!\n" << std::endl;
    
    printMenu({
        "Add Game Content",
        "Logout"
    });
    
    int choice = getChoice(2);
    switch (choice) {
        case 1: addGameContent(); break;
        case 2: logout(); break;
    }
}

void ConsoleClient::addGameContent() {
    clearScreen();
    printHeader("Add Game Content");
    
    printMenu({
        "Word Matching",
        "Sentence Matching",
        "Picture Matching"
    });
    
    int choice = getChoice(3);
    std::string gameType;
    std::string itemData;
    
    switch (choice) {
        case 1:
            gameType = "Word Matching";
            {
                std::string word1 = getInput("Word 1: ");
                std::string word2 = getInput("Word 2/Definition: ");
                itemData = word1 + "=" + word2;
            }
            break;
        case 2:
            gameType = "Sentence Matching";
            {
                std::string sent1 = getInput("Sentence 1: ");
                std::string sent2 = getInput("Sentence 2/Completion: ");
                itemData = sent1 + "=" + sent2;
            }
            break;
        case 3:
            gameType = "Picture Matching";
            {
                std::string imagePath = getInput("Image path: ");
                std::string text = getInput("Corresponding text: ");
                itemData = imagePath + "=" + text;
            }
            break;
    }
    
    // Send to server
    std::string payload = gameType + "|" + itemData;
    Message request(MessageType::ADD_GAME_ITEM_REQUEST, payload);
    Message response = client_->sendMessageSync(request);
    
    if (response.header.type == MessageType::ADD_GAME_ITEM_SUCCESS) {
        printSuccess("✓ Game item added successfully!");
    } else {
        printError("✗ Failed to add game item");
    }
    
    pause();
}

// ==================== UI Helpers ====================
void ConsoleClient::clearScreen() {
    #ifdef _WIN32
        [[maybe_unused]] int ret = system("cls");
    #else
        [[maybe_unused]] int ret = system("clear");
    #endif
}

void ConsoleClient::printHeader(const std::string& title) {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  " << std::left << std::setw(51) << title << "  ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

void ConsoleClient::printMenu(const std::vector<std::string>& options) {
    std::cout << "\nMenu:\n" << std::endl;
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << options[i] << std::endl;
    }
    std::cout << std::endl;
}

int ConsoleClient::getChoice(int max) {
    int choice;
    while (true) {
        std::cout << "Enter choice (1-" << max << "): ";
        if (std::cin >> choice && choice >= 1 && choice <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return choice;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid choice. Please try again." << std::endl;
    }
}

std::string ConsoleClient::getInput(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

std::string ConsoleClient::getPassword(const std::string& prompt) {
    std::cout << prompt;
    std::string password;
    
    #ifdef _WIN32
        char ch;
        while ((ch = _getch()) != '\r') {
            if (ch == '\b' && !password.empty()) {
                password.pop_back();
                std::cout << "\b \b";
            } else if (ch != '\b') {
                password += ch;
                std::cout << '*';
            }
        }
        std::cout << std::endl;
    #else
        termios oldt;
        tcgetattr(STDIN_FILENO, &oldt);
        termios newt = oldt;
        newt.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        std::getline(std::cin, password);
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        std::cout << std::endl;
    #endif
    
    return password;
}

void ConsoleClient::pause() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void ConsoleClient::printSuccess(const std::string& message) {
    std::cout << "\n✓ " << message << std::endl;
}

void ConsoleClient::printError(const std::string& message) {
    std::cout << "\n✗ " << message << std::endl;
}

