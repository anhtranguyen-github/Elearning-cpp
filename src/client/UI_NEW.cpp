#include "UI.hpp"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), authenticated_(false) {
    
    client_ = std::make_unique<Client>();
    
    setupUI();
    
    // Start on login screen
    navigateToScreen(SCREEN_LOGIN);
    updateConnectionStatus(false);
    
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
    setWindowTitle("English Learning Platform");
    setMinimumSize(1024, 768);
    
    // Create central widget with stacked layout for screen navigation
    stackedWidget_ = new QStackedWidget(this);
    setCentralWidget(stackedWidget_);
    
    // Setup all screens
    setupLoginScreen();           // S-1
    setupProfileScreen();         // S-2
    setupDashboardScreen();       // S-3
    setupLessonListScreen();      // S-4
    setupLessonViewScreen();      // S-5
    setupExerciseScreen();        // S-6
    setupSpeakingScreen();        // S-7
    setupScoreFeedbackScreen();   // S-8
    setupGamesScreen();           // S-9
    setupChatScreen();            // S-10
    
    // Teacher screens
    setupTeacherDashboard();
    setupFeedbackInboxScreen();   // T-1
    setupProvideFeedbackScreen(); // T-2
    setupTeacherChatScreen();     // T-3
    
    // Admin screens
    setupAdminDashboard();
    setupGameContentScreen();     // A-1
}

// ==================== S-1: Login/Registration Screen ====================
void MainWindow::setupLoginScreen() {
    loginScreen_ = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(loginScreen_);
    mainLayout->setAlignment(Qt::AlignCenter);
    
    // Title
    QLabel* titleLabel = new QLabel("English Learning Platform");
    QFont titleFont;
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // Connection status
    connectionStatusLabel_ = new QLabel("● Disconnected");
    connectionStatusLabel_->setStyleSheet("QLabel { color: red; font-size: 14px; }");
    connectionStatusLabel_->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(connectionStatusLabel_);
    
    mainLayout->addSpacing(20);
    
    // Login panel
    loginPanel_ = new QWidget();
    QVBoxLayout* loginLayout = new QVBoxLayout(loginPanel_);
    
    QGroupBox* loginGroup = new QGroupBox("Login");
    QFormLayout* loginForm = new QFormLayout();
    
    loginUsername_ = new QLineEdit();
    loginUsername_->setPlaceholderText("Username or Email");
    loginForm->addRow("Username:", loginUsername_);
    
    loginPassword_ = new QLineEdit();
    loginPassword_->setEchoMode(QLineEdit::Password);
    loginPassword_->setPlaceholderText("Password");
    loginForm->addRow("Password:", loginPassword_);
    
    loginButton_ = new QPushButton("Log In");
    loginButton_->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 10px; font-size: 14px; }");
    connect(loginButton_, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    
    switchToRegisterButton_ = new QPushButton("Don't have an account? Register");
    switchToRegisterButton_->setStyleSheet("QPushButton { border: none; color: #2196F3; }");
    connect(switchToRegisterButton_, &QPushButton::clicked, this, &MainWindow::onSwitchToRegister);
    
    loginForm->addRow(loginButton_);
    loginGroup->setLayout(loginForm);
    loginLayout->addWidget(loginGroup);
    loginLayout->addWidget(switchToRegisterButton_);
    
    // Register panel
    registerPanel_ = new QWidget();
    QVBoxLayout* registerLayout = new QVBoxLayout(registerPanel_);
    
    QGroupBox* registerGroup = new QGroupBox("Registration");
    QFormLayout* registerForm = new QFormLayout();
    
    registerUsername_ = new QLineEdit();
    registerUsername_->setPlaceholderText("Choose a username");
    registerForm->addRow("Username:", registerUsername_);
    
    registerEmail_ = new QLineEdit();
    registerEmail_->setPlaceholderText("your.email@example.com");
    registerForm->addRow("Email:", registerEmail_);
    
    registerPassword_ = new QLineEdit();
    registerPassword_->setEchoMode(QLineEdit::Password);
    registerPassword_->setPlaceholderText("Enter password");
    registerForm->addRow("Password:", registerPassword_);
    
    registerConfirmPassword_ = new QLineEdit();
    registerConfirmPassword_->setEchoMode(QLineEdit::Password);
    registerConfirmPassword_->setPlaceholderText("Confirm password");
    registerForm->addRow("Confirm:", registerConfirmPassword_);
    
    registerRoleCombo_ = new QComboBox();
    registerRoleCombo_->addItem("Student", static_cast<int>(UserRole::STUDENT));
    registerRoleCombo_->addItem("Teacher", static_cast<int>(UserRole::TEACHER));
    registerRoleCombo_->addItem("Admin", static_cast<int>(UserRole::ADMIN));
    registerForm->addRow("Role:", registerRoleCombo_);
    
    registerButton_ = new QPushButton("Register");
    registerButton_->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 10px; font-size: 14px; }");
    connect(registerButton_, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);
    
    switchToLoginButton_ = new QPushButton("Already have an account? Login");
    switchToLoginButton_->setStyleSheet("QPushButton { border: none; color: #4CAF50; }");
    connect(switchToLoginButton_, &QPushButton::clicked, this, &MainWindow::onSwitchToLogin);
    
    registerForm->addRow(registerButton_);
    registerGroup->setLayout(registerForm);
    registerLayout->addWidget(registerGroup);
    registerLayout->addWidget(switchToLoginButton_);
    
    registerPanel_->hide();  // Start with login visible
    
    // Add panels to main layout
    QWidget* formContainer = new QWidget();
    formContainer->setMaximumWidth(400);
    QVBoxLayout* containerLayout = new QVBoxLayout(formContainer);
    containerLayout->addWidget(loginPanel_);
    containerLayout->addWidget(registerPanel_);
    
    mainLayout->addWidget(formContainer, 0, Qt::AlignCenter);
    mainLayout->addStretch();
    
    stackedWidget_->addWidget(loginScreen_);  // Index 0
}

// ==================== S-2: Profile/Setup Screen ====================
void MainWindow::setupProfileScreen() {
    profileScreen_ = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(profileScreen_);
    
    QLabel* titleLabel = new QLabel("Initial Setup & Profile");
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);
    
    // Level Selection (2.1)
    QGroupBox* levelGroup = new QGroupBox("Select Your English Proficiency Level");
    QVBoxLayout* levelLayout = new QVBoxLayout();
    
    levelCombo_ = new QComboBox();
    levelCombo_->addItem("Beginner", static_cast<int>(ProficiencyLevel::BEGINNER));
    levelCombo_->addItem("Intermediate", static_cast<int>(ProficiencyLevel::INTERMEDIATE));
    levelCombo_->addItem("Advanced", static_cast<int>(ProficiencyLevel::ADVANCED));
    
    levelLayout->addWidget(new QLabel("Choose your current level:"));
    levelLayout->addWidget(levelCombo_);
    levelGroup->setLayout(levelLayout);
    layout->addWidget(levelGroup);
    
    // Content File Location (7.3)
    QGroupBox* settingsGroup = new QGroupBox("Settings");
    QFormLayout* settingsForm = new QFormLayout();
    
    QHBoxLayout* contentLocLayout = new QHBoxLayout();
    contentLocationEdit_ = new QLineEdit();
    contentLocationEdit_->setPlaceholderText("Local address of content files");
    browseContentButton_ = new QPushButton("Browse...");
    connect(browseContentButton_, &QPushButton::clicked, this, &MainWindow::onBrowseContentLocation);
    contentLocLayout->addWidget(contentLocationEdit_);
    contentLocLayout->addWidget(browseContentButton_);
    
    settingsForm->addRow("Content Location:", contentLocLayout);
    settingsGroup->setLayout(settingsForm);
    layout->addWidget(settingsGroup);
    
    // Save button
    saveProfileButton_ = new QPushButton("Save & Continue");
    saveProfileButton_->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 10px; }");
    connect(saveProfileButton_, &QPushButton::clicked, this, &MainWindow::onSaveProfileClicked);
    layout->addWidget(saveProfileButton_);
    
    layout->addStretch();
    
    stackedWidget_->addWidget(profileScreen_);  // Index 1
}

// ==================== S-3: Main Dashboard ====================
void MainWindow::setupDashboardScreen() {
    dashboardScreen_ = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(dashboardScreen_);
    
    // Welcome header
    dashWelcomeLabel_ = new QLabel("Welcome!");
    QFont welcomeFont;
    welcomeFont.setPointSize(20);
    welcomeFont.setBold(true);
    dashWelcomeLabel_->setFont(welcomeFont);
    layout->addWidget(dashWelcomeLabel_);
    
    // Status section
    QHBoxLayout* statusLayout = new QHBoxLayout();
    dashLevelLabel_ = new QLabel("Level: Beginner");
    dashScoreLabel_ = new QLabel("Total Score: 0");
    statusLayout->addWidget(dashLevelLabel_);
    statusLayout->addStretch();
    statusLayout->addWidget(dashScoreLabel_);
    layout->addLayout(statusLayout);
    
    layout->addSpacing(20);
    
    // Navigation buttons
    QGridLayout* navGrid = new QGridLayout();
    navGrid->setSpacing(15);
    
    dashLessonsButton_ = new QPushButton("📚 Lessons");
    dashLessonsButton_->setMinimumHeight(100);
    dashLessonsButton_->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font-size: 16px; font-weight: bold; }");
    connect(dashLessonsButton_, &QPushButton::clicked, this, &MainWindow::onDashboardLessonsClicked);
    
    dashExercisesButton_ = new QPushButton("✏️ Exercises");
    dashExercisesButton_->setMinimumHeight(100);
    dashExercisesButton_->setStyleSheet("QPushButton { background-color: #FF9800; color: white; font-size: 16px; font-weight: bold; }");
    connect(dashExercisesButton_, &QPushButton::clicked, this, &MainWindow::onDashboardExercisesClicked);
    
    dashGamesButton_ = new QPushButton("🎮 Games");
    dashGamesButton_->setMinimumHeight(100);
    dashGamesButton_->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-size: 16px; font-weight: bold; }");
    connect(dashGamesButton_, &QPushButton::clicked, this, &MainWindow::onDashboardGamesClicked);
    
    dashChatButton_ = new QPushButton("💬 Chat");
    dashChatButton_->setMinimumHeight(100);
    dashChatButton_->setStyleSheet("QPushButton { background-color: #9C27B0; color: white; font-size: 16px; font-weight: bold; }");
    connect(dashChatButton_, &QPushButton::clicked, this, &MainWindow::onDashboardChatClicked);
    
    dashProfileButton_ = new QPushButton("⚙️ Profile & Settings");
    dashProfileButton_->setMinimumHeight(100);
    dashProfileButton_->setStyleSheet("QPushButton { background-color: #607D8B; color: white; font-size: 16px; font-weight: bold; }");
    connect(dashProfileButton_, &QPushButton::clicked, this, &MainWindow::onDashboardProfileClicked);
    
    logoutButton_ = new QPushButton("🚪 Logout");
    logoutButton_->setMinimumHeight(100);
    logoutButton_->setStyleSheet("QPushButton { background-color: #F44336; color: white; font-size: 16px; font-weight: bold; }");
    connect(logoutButton_, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
    
    navGrid->addWidget(dashLessonsButton_, 0, 0);
    navGrid->addWidget(dashExercisesButton_, 0, 1);
    navGrid->addWidget(dashGamesButton_, 1, 0);
    navGrid->addWidget(dashChatButton_, 1, 1);
    navGrid->addWidget(dashProfileButton_, 2, 0);
    navGrid->addWidget(logoutButton_, 2, 1);
    
    layout->addLayout(navGrid);
    
    layout->addSpacing(20);
    
    // Recent Activity
    QGroupBox* activityGroup = new QGroupBox("Recent Activity");
    QVBoxLayout* activityLayout = new QVBoxLayout();
    dashActivityText_ = new QTextEdit();
    dashActivityText_->setReadOnly(true);
    dashActivityText_->setMaximumHeight(150);
    dashActivityText_->setText("No recent activity.");
    activityLayout->addWidget(dashActivityText_);
    activityGroup->setLayout(activityLayout);
    layout->addWidget(activityGroup);
    
    layout->addStretch();
    
    stackedWidget_->addWidget(dashboardScreen_);  // Index 2
}

// Continue with remaining screens in next part due to length...

