#include "UI.hpp"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), authenticated_(false) {
    
    client_ = std::make_unique<Client>();
    
    setupUI();
    updateConnectionStatus(false);
    updateAuthStatus(false);
    
    // Setup heartbeat timer
    heartbeatTimer_ = new QTimer(this);
    connect(heartbeatTimer_, &QTimer::timeout, this, &MainWindow::onHeartbeatTimer);
}

MainWindow::~MainWindow() {
    if (client_->isConnected()) {
        client_->disconnect();
    }
}

void MainWindow::setupUI() {
    setWindowTitle("English Learning Platform - Client");
    setMinimumSize(900, 700);
    
    // Create central widget and main layout
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Create tab widget
    tabWidget_ = new QTabWidget(this);
    mainLayout->addWidget(tabWidget_);
    
    // Setup all tabs
    setupConnectionTab();
    setupAuthTab();
    setupStudyTab();
    setupExerciseTab();
    setupGameTab();
    setupCommunicationTab();
    setupScoreTab();
}

void MainWindow::setupConnectionTab() {
    QWidget* connectionTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(connectionTab);
    
    QGroupBox* connectionGroup = new QGroupBox("Server Connection");
    QFormLayout* formLayout = new QFormLayout();
    
    serverAddressEdit_ = new QLineEdit("127.0.0.1");
    serverPortEdit_ = new QLineEdit("8080");
    
    formLayout->addRow("Server Address:", serverAddressEdit_);
    formLayout->addRow("Server Port:", serverPortEdit_);
    
    connectionGroup->setLayout(formLayout);
    layout->addWidget(connectionGroup);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    connectButton_ = new QPushButton("Connect");
    disconnectButton_ = new QPushButton("Disconnect");
    disconnectButton_->setEnabled(false);
    
    connect(connectButton_, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(disconnectButton_, &QPushButton::clicked, this, &MainWindow::onDisconnectClicked);
    
    buttonLayout->addWidget(connectButton_);
    buttonLayout->addWidget(disconnectButton_);
    layout->addLayout(buttonLayout);
    
    connectionStatusLabel_ = new QLabel("Status: Disconnected");
    layout->addWidget(connectionStatusLabel_);
    
    layout->addStretch();
    
    tabWidget_->addTab(connectionTab, "Connection");
}

void MainWindow::setupAuthTab() {
    QWidget* authTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(authTab);
    
    QGroupBox* authGroup = new QGroupBox("User Authentication");
    QFormLayout* formLayout = new QFormLayout();
    
    usernameEdit_ = new QLineEdit();
    passwordEdit_ = new QLineEdit();
    passwordEdit_->setEchoMode(QLineEdit::Password);
    
    roleComboBox_ = new QComboBox();
    roleComboBox_->addItem("Student", static_cast<int>(UserRole::STUDENT));
    roleComboBox_->addItem("Teacher", static_cast<int>(UserRole::TEACHER));
    roleComboBox_->addItem("Admin", static_cast<int>(UserRole::ADMIN));
    
    formLayout->addRow("Username:", usernameEdit_);
    formLayout->addRow("Password:", passwordEdit_);
    formLayout->addRow("Role:", roleComboBox_);
    
    authGroup->setLayout(formLayout);
    layout->addWidget(authGroup);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    loginButton_ = new QPushButton("Login");
    registerButton_ = new QPushButton("Register");
    logoutButton_ = new QPushButton("Logout");
    logoutButton_->setEnabled(false);
    
    connect(loginButton_, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(registerButton_, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);
    connect(logoutButton_, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
    
    buttonLayout->addWidget(loginButton_);
    buttonLayout->addWidget(registerButton_);
    buttonLayout->addWidget(logoutButton_);
    layout->addLayout(buttonLayout);
    
    authStatusLabel_ = new QLabel("Status: Not Authenticated");
    layout->addWidget(authStatusLabel_);
    
    layout->addStretch();
    
    tabWidget_->addTab(authTab, "Authentication");
}

void MainWindow::setupStudyTab() {
    QWidget* studyTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(studyTab);
    
    // Level selection
    QGroupBox* levelGroup = new QGroupBox("Proficiency Level");
    QHBoxLayout* levelLayout = new QHBoxLayout();
    
    levelComboBox_ = new QComboBox();
    levelComboBox_->addItem("Beginner", static_cast<int>(ProficiencyLevel::BEGINNER));
    levelComboBox_->addItem("Intermediate", static_cast<int>(ProficiencyLevel::INTERMEDIATE));
    levelComboBox_->addItem("Advanced", static_cast<int>(ProficiencyLevel::ADVANCED));
    
    setLevelButton_ = new QPushButton("Set Level");
    connect(setLevelButton_, &QPushButton::clicked, this, &MainWindow::onSetLevelClicked);
    
    levelLayout->addWidget(levelComboBox_);
    levelLayout->addWidget(setLevelButton_);
    levelGroup->setLayout(levelLayout);
    layout->addWidget(levelGroup);
    
    // Lesson list
    QGroupBox* lessonGroup = new QGroupBox("Lessons");
    QVBoxLayout* lessonLayout = new QVBoxLayout();
    
    lessonListWidget_ = new QListWidget();
    connect(lessonListWidget_, &QListWidget::itemClicked, this, &MainWindow::onLessonSelected);
    
    QHBoxLayout* lessonButtonLayout = new QHBoxLayout();
    refreshLessonsButton_ = new QPushButton("Refresh Lessons");
    viewLessonButton_ = new QPushButton("View Lesson");
    viewLessonButton_->setEnabled(false);
    
    connect(refreshLessonsButton_, &QPushButton::clicked, this, &MainWindow::onRefreshLessonsClicked);
    connect(viewLessonButton_, &QPushButton::clicked, this, &MainWindow::onViewLessonClicked);
    
    lessonButtonLayout->addWidget(refreshLessonsButton_);
    lessonButtonLayout->addWidget(viewLessonButton_);
    
    lessonLayout->addWidget(lessonListWidget_);
    lessonLayout->addLayout(lessonButtonLayout);
    lessonGroup->setLayout(lessonLayout);
    layout->addWidget(lessonGroup);
    
    // Lesson content
    QGroupBox* contentGroup = new QGroupBox("Lesson Content");
    QVBoxLayout* contentLayout = new QVBoxLayout();
    
    lessonContentText_ = new QTextEdit();
    lessonContentText_->setReadOnly(true);
    contentLayout->addWidget(lessonContentText_);
    
    contentGroup->setLayout(contentLayout);
    layout->addWidget(contentGroup);
    
    tabWidget_->addTab(studyTab, "Study");
}

void MainWindow::setupExerciseTab() {
    QWidget* exerciseTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(exerciseTab);
    
    // Quiz submission
    QGroupBox* quizGroup = new QGroupBox("Quiz Submission");
    QVBoxLayout* quizLayout = new QVBoxLayout();
    
    QFormLayout* quizFormLayout = new QFormLayout();
    quizIdEdit_ = new QLineEdit();
    quizAnswersEdit_ = new QTextEdit();
    quizAnswersEdit_->setMaximumHeight(100);
    
    quizFormLayout->addRow("Quiz ID:", quizIdEdit_);
    quizFormLayout->addRow("Answers:", quizAnswersEdit_);
    
    submitQuizButton_ = new QPushButton("Submit Quiz");
    connect(submitQuizButton_, &QPushButton::clicked, this, &MainWindow::onSubmitQuizClicked);
    
    quizLayout->addLayout(quizFormLayout);
    quizLayout->addWidget(submitQuizButton_);
    quizGroup->setLayout(quizLayout);
    layout->addWidget(quizGroup);
    
    // Exercise submission
    QGroupBox* exerciseGroup = new QGroupBox("Exercise Submission");
    QVBoxLayout* exerciseLayout = new QVBoxLayout();
    
    QFormLayout* exerciseFormLayout = new QFormLayout();
    exerciseIdEdit_ = new QLineEdit();
    exerciseContentEdit_ = new QTextEdit();
    exerciseContentEdit_->setMaximumHeight(100);
    
    exerciseFormLayout->addRow("Exercise ID:", exerciseIdEdit_);
    exerciseFormLayout->addRow("Content:", exerciseContentEdit_);
    
    submitExerciseButton_ = new QPushButton("Submit Exercise");
    connect(submitExerciseButton_, &QPushButton::clicked, this, &MainWindow::onSubmitExerciseClicked);
    
    exerciseLayout->addLayout(exerciseFormLayout);
    exerciseLayout->addWidget(submitExerciseButton_);
    exerciseGroup->setLayout(exerciseLayout);
    layout->addWidget(exerciseGroup);
    
    exerciseScoreLabel_ = new QLabel("Score: -");
    layout->addWidget(exerciseScoreLabel_);
    
    layout->addStretch();
    
    tabWidget_->addTab(exerciseTab, "Exercises");
}

void MainWindow::setupGameTab() {
    QWidget* gameTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(gameTab);
    
    QGroupBox* gameGroup = new QGroupBox("Game");
    QVBoxLayout* gameLayout = new QVBoxLayout();
    
    QHBoxLayout* gameSelectLayout = new QHBoxLayout();
    gameTypeComboBox_ = new QComboBox();
    gameTypeComboBox_->addItem("Word Matching");
    gameTypeComboBox_->addItem("Sentence Matching");
    gameTypeComboBox_->addItem("Picture Matching");
    
    startGameButton_ = new QPushButton("Start Game");
    connect(startGameButton_, &QPushButton::clicked, this, &MainWindow::onStartGameClicked);
    
    gameSelectLayout->addWidget(new QLabel("Game Type:"));
    gameSelectLayout->addWidget(gameTypeComboBox_);
    gameSelectLayout->addWidget(startGameButton_);
    
    gameStateText_ = new QTextEdit();
    gameStateText_->setReadOnly(true);
    
    QHBoxLayout* moveLayout = new QHBoxLayout();
    gameMoveEdit_ = new QLineEdit();
    sendMoveButton_ = new QPushButton("Send Move");
    sendMoveButton_->setEnabled(false);
    connect(sendMoveButton_, &QPushButton::clicked, this, &MainWindow::onSendMoveClicked);
    
    moveLayout->addWidget(new QLabel("Move:"));
    moveLayout->addWidget(gameMoveEdit_);
    moveLayout->addWidget(sendMoveButton_);
    
    gameLayout->addLayout(gameSelectLayout);
    gameLayout->addWidget(gameStateText_);
    gameLayout->addLayout(moveLayout);
    gameGroup->setLayout(gameLayout);
    layout->addWidget(gameGroup);
    
    layout->addStretch();
    
    tabWidget_->addTab(gameTab, "Games");
}

void MainWindow::setupCommunicationTab() {
    QWidget* commTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(commTab);
    
    // Chat
    QGroupBox* chatGroup = new QGroupBox("Chat");
    QVBoxLayout* chatLayout = new QVBoxLayout();
    
    chatHistoryText_ = new QTextEdit();
    chatHistoryText_->setReadOnly(true);
    
    QFormLayout* chatFormLayout = new QFormLayout();
    chatRecipientEdit_ = new QLineEdit();
    chatMessageEdit_ = new QTextEdit();
    chatMessageEdit_->setMaximumHeight(60);
    
    chatFormLayout->addRow("Recipient:", chatRecipientEdit_);
    chatFormLayout->addRow("Message:", chatMessageEdit_);
    
    sendChatButton_ = new QPushButton("Send Message");
    connect(sendChatButton_, &QPushButton::clicked, this, &MainWindow::onSendChatClicked);
    
    chatLayout->addWidget(new QLabel("Chat History:"));
    chatLayout->addWidget(chatHistoryText_);
    chatLayout->addLayout(chatFormLayout);
    chatLayout->addWidget(sendChatButton_);
    chatGroup->setLayout(chatLayout);
    layout->addWidget(chatGroup);
    
    // Voice call
    QGroupBox* voiceGroup = new QGroupBox("Voice Call");
    QHBoxLayout* voiceLayout = new QHBoxLayout();
    
    voiceCallTargetEdit_ = new QLineEdit();
    voiceCallButton_ = new QPushButton("Initiate Voice Call");
    connect(voiceCallButton_, &QPushButton::clicked, this, &MainWindow::onVoiceCallClicked);
    
    voiceLayout->addWidget(new QLabel("Target User:"));
    voiceLayout->addWidget(voiceCallTargetEdit_);
    voiceLayout->addWidget(voiceCallButton_);
    voiceGroup->setLayout(voiceLayout);
    layout->addWidget(voiceGroup);
    
    layout->addStretch();
    
    tabWidget_->addTab(commTab, "Communication");
}

void MainWindow::setupScoreTab() {
    QWidget* scoreTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(scoreTab);
    
    // Score
    QGroupBox* scoreGroup = new QGroupBox("Score");
    QVBoxLayout* scoreLayout = new QVBoxLayout();
    
    scoreLabel_ = new QLabel("Total Score: -");
    QFont font = scoreLabel_->font();
    font.setPointSize(14);
    font.setBold(true);
    scoreLabel_->setFont(font);
    
    refreshScoreButton_ = new QPushButton("Refresh Score");
    connect(refreshScoreButton_, &QPushButton::clicked, this, &MainWindow::onRefreshScoreClicked);
    
    scoreLayout->addWidget(scoreLabel_);
    scoreLayout->addWidget(refreshScoreButton_);
    scoreGroup->setLayout(scoreLayout);
    layout->addWidget(scoreGroup);
    
    // Feedback
    QGroupBox* feedbackGroup = new QGroupBox("Feedback");
    QVBoxLayout* feedbackLayout = new QVBoxLayout();
    
    feedbackText_ = new QTextEdit();
    feedbackText_->setReadOnly(true);
    
    refreshFeedbackButton_ = new QPushButton("Refresh Feedback");
    connect(refreshFeedbackButton_, &QPushButton::clicked, this, &MainWindow::onRefreshFeedbackClicked);
    
    feedbackLayout->addWidget(feedbackText_);
    feedbackLayout->addWidget(refreshFeedbackButton_);
    feedbackGroup->setLayout(feedbackLayout);
    layout->addWidget(feedbackGroup);
    
    tabWidget_->addTab(scoreTab, "Score & Feedback");
}

// Connection handlers
void MainWindow::onConnectClicked() {
    std::string address = serverAddressEdit_->text().toStdString();
    int port = serverPortEdit_->text().toInt();
    
    if (client_->connect(address, port)) {
        updateConnectionStatus(true);
        heartbeatTimer_->start(30000); // 30 second heartbeat
        showMessage("Success", "Connected to server");
    } else {
        showMessage("Error", "Failed to connect to server");
    }
}

void MainWindow::onDisconnectClicked() {
    client_->disconnect();
    heartbeatTimer_->stop();
    updateConnectionStatus(false);
    updateAuthStatus(false);
    showMessage("Info", "Disconnected from server");
}

// Authentication handlers
void MainWindow::onLoginClicked() {
    if (!client_->isConnected()) {
        showMessage("Error", "Not connected to server");
        return;
    }
    
    std::string username = usernameEdit_->text().toStdString();
    std::string password = passwordEdit_->text().toStdString();
    
    UserData userData;
    if (client_->login(username, password, userData)) {
        authenticated_ = true;
        currentUser_ = userData;
        updateAuthStatus(true);
        showMessage("Success", "Logged in successfully");
    } else {
        showMessage("Error", "Login failed");
    }
}

void MainWindow::onRegisterClicked() {
    if (!client_->isConnected()) {
        showMessage("Error", "Not connected to server");
        return;
    }
    
    std::string username = usernameEdit_->text().toStdString();
    std::string password = passwordEdit_->text().toStdString();
    UserRole role = static_cast<UserRole>(roleComboBox_->currentData().toInt());
    
    if (client_->registerUser(username, password, role)) {
        showMessage("Success", "User registered successfully. Please login.");
    } else {
        showMessage("Error", "Registration failed");
    }
}

void MainWindow::onLogoutClicked() {
    if (client_->logout()) {
        authenticated_ = false;
        updateAuthStatus(false);
        showMessage("Info", "Logged out successfully");
    }
}

// Study handlers
void MainWindow::onSetLevelClicked() {
    if (!authenticated_) {
        showMessage("Error", "Please login first");
        return;
    }
    
    ProficiencyLevel level = static_cast<ProficiencyLevel>(levelComboBox_->currentData().toInt());
    
    if (client_->setLevel(level)) {
        currentUser_.level = level;
        showMessage("Success", "Level set successfully");
    } else {
        showMessage("Error", "Failed to set level");
    }
}

void MainWindow::onRefreshLessonsClicked() {
    if (!authenticated_) {
        showMessage("Error", "Please login first");
        return;
    }
    
    std::vector<std::string> lessons = client_->getLessonList();
    
    lessonListWidget_->clear();
    for (const auto& lesson : lessons) {
        lessonListWidget_->addItem(QString::fromStdString(lesson));
    }
}

void MainWindow::onLessonSelected() {
    viewLessonButton_->setEnabled(true);
}

void MainWindow::onViewLessonClicked() {
    QListWidgetItem* item = lessonListWidget_->currentItem();
    if (!item) return;
    
    std::string lessonId = item->text().toStdString();
    // Extract ID from "id:title" format
    size_t pos = lessonId.find(':');
    if (pos != std::string::npos) {
        lessonId = lessonId.substr(0, pos);
    }
    
    std::string content = client_->getLessonContent(lessonId);
    lessonContentText_->setPlainText(QString::fromStdString(content));
}

// Exercise handlers
void MainWindow::onSubmitQuizClicked() {
    if (!authenticated_) {
        showMessage("Error", "Please login first");
        return;
    }
    
    std::string quizId = quizIdEdit_->text().toStdString();
    std::string answers = quizAnswersEdit_->toPlainText().toStdString();
    
    int score;
    if (client_->submitQuiz(quizId, answers, score)) {
        exerciseScoreLabel_->setText(QString("Score: +%1").arg(score));
        showMessage("Success", QString("Quiz submitted! Score: %1").arg(score));
    } else {
        showMessage("Error", "Failed to submit quiz");
    }
}

void MainWindow::onSubmitExerciseClicked() {
    if (!authenticated_) {
        showMessage("Error", "Please login first");
        return;
    }
    
    std::string exerciseId = exerciseIdEdit_->text().toStdString();
    std::string content = exerciseContentEdit_->toPlainText().toStdString();
    
    int score;
    if (client_->submitExercise(exerciseId, content, score)) {
        exerciseScoreLabel_->setText(QString("Score: +%1").arg(score));
        showMessage("Success", QString("Exercise submitted! Score: %1").arg(score));
    } else {
        showMessage("Error", "Failed to submit exercise");
    }
}

// Game handlers
void MainWindow::onStartGameClicked() {
    if (!authenticated_) {
        showMessage("Error", "Please login first");
        return;
    }
    
    std::string gameType = gameTypeComboBox_->currentText().toStdString();
    std::string gameData = client_->startGame(gameType);
    
    if (!gameData.empty()) {
        gameStateText_->setPlainText(QString::fromStdString("Game started!\n" + gameData));
        sendMoveButton_->setEnabled(true);
    } else {
        showMessage("Error", "Failed to start game");
    }
}

void MainWindow::onSendMoveClicked() {
    std::string move = gameMoveEdit_->text().toStdString();
    std::string response;
    
    if (client_->sendGameMove(move, response)) {
        gameStateText_->append(QString::fromStdString("\nMove: " + move + "\nResult: " + response));
        gameMoveEdit_->clear();
    } else {
        showMessage("Error", "Failed to send move");
    }
}

// Communication handlers
void MainWindow::onSendChatClicked() {
    if (!authenticated_) {
        showMessage("Error", "Please login first");
        return;
    }
    
    std::string recipient = chatRecipientEdit_->text().toStdString();
    std::string message = chatMessageEdit_->toPlainText().toStdString();
    
    if (client_->sendChatMessage(recipient, message)) {
        chatHistoryText_->append(QString("[You -> %1]: %2\n")
            .arg(QString::fromStdString(recipient))
            .arg(QString::fromStdString(message)));
        chatMessageEdit_->clear();
    } else {
        showMessage("Error", "Failed to send message");
    }
}

void MainWindow::onVoiceCallClicked() {
    if (!authenticated_) {
        showMessage("Error", "Please login first");
        return;
    }
    
    std::string target = voiceCallTargetEdit_->text().toStdString();
    
    if (client_->initiateVoiceCall(target)) {
        showMessage("Success", QString("Voice call initiated with %1")
            .arg(QString::fromStdString(target)));
    } else {
        showMessage("Error", "Failed to initiate voice call");
    }
}

// Score and feedback handlers
void MainWindow::onRefreshScoreClicked() {
    if (!authenticated_) {
        showMessage("Error", "Please login first");
        return;
    }
    
    int score = client_->getScore();
    scoreLabel_->setText(QString("Total Score: %1").arg(score));
}

void MainWindow::onRefreshFeedbackClicked() {
    if (!authenticated_) {
        showMessage("Error", "Please login first");
        return;
    }
    
    std::vector<std::string> feedbacks = client_->getFeedback();
    
    feedbackText_->clear();
    for (const auto& feedback : feedbacks) {
        feedbackText_->append(QString::fromStdString(feedback + "\n---\n"));
    }
}

// Timer handlers
void MainWindow::onHeartbeatTimer() {
    if (client_->isConnected()) {
        client_->sendHeartbeat();
    }
}

// UI update helpers
void MainWindow::updateConnectionStatus(bool connected) {
    if (connected) {
        connectionStatusLabel_->setText("Status: Connected");
        connectionStatusLabel_->setStyleSheet("color: green;");
        connectButton_->setEnabled(false);
        disconnectButton_->setEnabled(true);
    } else {
        connectionStatusLabel_->setText("Status: Disconnected");
        connectionStatusLabel_->setStyleSheet("color: red;");
        connectButton_->setEnabled(true);
        disconnectButton_->setEnabled(false);
    }
}

void MainWindow::updateAuthStatus(bool authenticated) {
    if (authenticated) {
        authStatusLabel_->setText(QString("Status: Authenticated as %1")
            .arg(QString::fromStdString(currentUser_.username)));
        authStatusLabel_->setStyleSheet("color: green;");
        loginButton_->setEnabled(false);
        registerButton_->setEnabled(false);
        logoutButton_->setEnabled(true);
    } else {
        authStatusLabel_->setText("Status: Not Authenticated");
        authStatusLabel_->setStyleSheet("color: red;");
        loginButton_->setEnabled(true);
        registerButton_->setEnabled(true);
        logoutButton_->setEnabled(false);
    }
}

void MainWindow::showMessage(const QString& title, const QString& message) {
    QMessageBox::information(this, title, message);
}

