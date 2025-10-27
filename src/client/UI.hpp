#ifndef UI_HPP
#define UI_HPP

#include "../../include/common.hpp"
#include "../../include/message_structs.hpp"
#include "Client.hpp"

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QComboBox>
#include <QListWidget>
#include <QMessageBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QTimer>

// Main application window using Qt Widgets
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    // Connection slots
    void onConnectClicked();
    void onDisconnectClicked();
    
    // Authentication slots
    void onLoginClicked();
    void onRegisterClicked();
    void onLogoutClicked();
    
    // Study slots
    void onSetLevelClicked();
    void onRefreshLessonsClicked();
    void onLessonSelected();
    void onViewLessonClicked();
    
    // Exercise slots
    void onSubmitQuizClicked();
    void onSubmitExerciseClicked();
    
    // Game slots
    void onStartGameClicked();
    void onSendMoveClicked();
    
    // Communication slots
    void onSendChatClicked();
    void onVoiceCallClicked();
    
    // Score and feedback slots
    void onRefreshScoreClicked();
    void onRefreshFeedbackClicked();
    
    // Timer slots
    void onHeartbeatTimer();

private:
    void setupUI();
    void setupConnectionTab();
    void setupAuthTab();
    void setupStudyTab();
    void setupExerciseTab();
    void setupGameTab();
    void setupCommunicationTab();
    void setupScoreTab();
    
    void updateConnectionStatus(bool connected);
    void updateAuthStatus(bool authenticated);
    void showMessage(const QString& title, const QString& message);
    
    // Client instance
    std::unique_ptr<Client> client_;
    
    // User data
    bool authenticated_;
    UserData currentUser_;
    
    // Main UI components
    QTabWidget* tabWidget_;
    
    // Connection tab
    QLineEdit* serverAddressEdit_;
    QLineEdit* serverPortEdit_;
    QPushButton* connectButton_;
    QPushButton* disconnectButton_;
    QLabel* connectionStatusLabel_;
    
    // Authentication tab
    QLineEdit* usernameEdit_;
    QLineEdit* passwordEdit_;
    QComboBox* roleComboBox_;
    QPushButton* loginButton_;
    QPushButton* registerButton_;
    QPushButton* logoutButton_;
    QLabel* authStatusLabel_;
    
    // Study tab
    QComboBox* levelComboBox_;
    QPushButton* setLevelButton_;
    QListWidget* lessonListWidget_;
    QPushButton* refreshLessonsButton_;
    QPushButton* viewLessonButton_;
    QTextEdit* lessonContentText_;
    
    // Exercise tab
    QLineEdit* quizIdEdit_;
    QTextEdit* quizAnswersEdit_;
    QPushButton* submitQuizButton_;
    QLineEdit* exerciseIdEdit_;
    QTextEdit* exerciseContentEdit_;
    QPushButton* submitExerciseButton_;
    QLabel* exerciseScoreLabel_;
    
    // Game tab
    QComboBox* gameTypeComboBox_;
    QPushButton* startGameButton_;
    QTextEdit* gameStateText_;
    QLineEdit* gameMoveEdit_;
    QPushButton* sendMoveButton_;
    
    // Communication tab
    QLineEdit* chatRecipientEdit_;
    QTextEdit* chatMessageEdit_;
    QPushButton* sendChatButton_;
    QTextEdit* chatHistoryText_;
    QLineEdit* voiceCallTargetEdit_;
    QPushButton* voiceCallButton_;
    
    // Score tab
    QLabel* scoreLabel_;
    QPushButton* refreshScoreButton_;
    QTextEdit* feedbackText_;
    QPushButton* refreshFeedbackButton_;
    
    // Timer
    QTimer* heartbeatTimer_;
};

#endif // UI_HPP

