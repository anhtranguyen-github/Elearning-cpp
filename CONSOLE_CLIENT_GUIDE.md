# Console Client Guide

## Overview

The **Console Client** is a terminal-based client that provides full functionality without requiring Qt GUI. It's perfect for:
- Running on servers without GUI
- Quick testing
- Scripting and automation
- Lightweight usage

---

## ✨ Features

### Automatic Role Detection
- **No role selection needed!** 
- Server automatically assigns role based on username/password
- Different menus appear based on your role (Student/Teacher/Admin)

### Full Functionality
- All features available in Qt GUI client
- Menu-driven interface
- Password hidden during input
- Role-based navigation

---

## 🚀 How to Run

### Start the Server (Terminal 1):
```bash
cd /mnt/c/Users/tra01/OneDrive/Desktop/socker/project
./server
```

### Start Console Client (Terminal 2):
```bash
./console_client
```

---

## 📖 Usage Guide

### 1. Connect to Server

When you start the console client:
```
╔═══════════════════════════════════════════════════════╗
║  English Learning Platform - Console Client          ║
╚═══════════════════════════════════════════════════════╝

Welcome to the English Learning Platform!

Menu:

  1. Connect to Server
  2. Exit

Enter choice (1-2): 1
```

Enter server details:
```
Server address [127.0.0.1]: <press Enter for default>
Server port [8080]: <press Enter for default>

Connecting to 127.0.0.1:8080...
✓ Connected to server successfully!
```

---

### 2. Register or Login

After connecting:
```
Menu:

  1. Login
  2. Register New Account
  3. Disconnect

Enter choice (1-3): 
```

#### Option A: Register New Account
```
Choose username: student1
Choose password: ****** (hidden)
Confirm password: ****** (hidden)

Select role:
  1. Student
  2. Teacher
  3. Admin

Enter choice (1-3): 1

✓ Registration successful! You can now login.
```

#### Option B: Login with Existing Account
```
Username: admin
Password: ********* (hidden)

Authenticating...

✓ Login successful!

Welcome, admin!
Role: Admin
Level: Beginner
Score: 0
```

**Default Accounts:**
- Username: `admin`, Password: `admin123` (Admin)
- Username: `teacher1`, Password: `teacher123` (Teacher)

---

### 3. Student Menu

After logging in as a **Student**, you'll see:

```
╔═══════════════════════════════════════════════════════╗
║  Student Dashboard                                    ║
╚═══════════════════════════════════════════════════════╝

Welcome, student1!
Level: Beginner | Score: 0

Menu:

  1. Set Proficiency Level
  2. Browse Lessons
  3. Submit Quiz
  4. Submit Exercise
  5. Play Game
  6. Chat
  7. View Score & Feedback
  8. Logout
```

#### Student Features:

**1. Set Proficiency Level**
```
Current level: Beginner

Menu:
  1. Beginner
  2. Intermediate
  3. Advanced

Enter choice (1-3): 2

✓ Level updated successfully!
```

**2. Browse Lessons**
```
Fetching lessons for your level...

Available Lessons:

  1. lesson_i1:Travel and Transportation
  2. lesson_i2:Food and Cooking
  3. lesson_i3:Work and Career

  0. Back

Enter choice (0-3): 1

╔═══════════════════════════════════════════════════════╗
║  Lesson: lesson_i1:Travel and Transportation          ║
╚═══════════════════════════════════════════════════════╝

Content for lesson: lesson_i1
Video: video_url
Audio: audio_url
Text: lesson_text
```

**3. Submit Quiz**
```
Quiz ID: quiz_001
Your answers: A,B,C,D

Submitting quiz...
✓ Quiz submitted successfully!
Score: +10 points
```

**4. Submit Exercise**
```
Exercise ID: essay_001

Enter your answer (press Enter twice to finish):

My essay about...
<Enter your text>
<Press Enter>
<Press Enter again to finish>

Submitting exercise...
✓ Exercise submitted successfully!
Score: +5 points
```

**5. Play Game**
```
Menu:
  1. Word Matching
  2. Sentence Matching
  3. Picture Matching

Enter choice (1-3): 1

Starting Word Matching...

Game started!
game_session_id_123|

Enter your move: match_word1_word2

Result: move_accepted|score:10
```

**6. Chat**
```
Recipient username: teacher1
Message: Hello, I need help with lesson 3

✓ Message sent to teacher1
```

**7. View Score & Feedback**
```
Total Score: 15 points

Fetching feedback...

Feedback:

  • Exercise: essay_001 | From: teacher1 | Good work! Keep practicing.
  • Exercise: quiz_001 | From: system | Correct answers: 4/5
```

---

### 4. Teacher Menu

After logging in as a **Teacher**:

```
╔═══════════════════════════════════════════════════════╗
║  Teacher Dashboard                                     ║
╚═══════════════════════════════════════════════════════╝

Welcome, Teacher teacher1!

Menu:

  1. Provide Feedback to Student
  2. Chat with Student
  3. Logout
```

#### Teacher Features:

**1. Provide Feedback**
```
Student username: student1
Exercise ID: essay_001

Enter feedback (press Enter twice to finish):

Great work on your essay! Your grammar has improved.
Consider working on your vocabulary.
<Press Enter>
<Press Enter again>

✓ Feedback sent to student1
```

**2. Chat with Student**
```
Student username: student1
Message: I'm available for questions today

✓ Message sent to student1
```

---

### 5. Admin Menu

After logging in as **Admin**:

```
╔═══════════════════════════════════════════════════════╗
║  Admin Dashboard                                       ║
╚═══════════════════════════════════════════════════════╝

Welcome, Admin admin!

Menu:

  1. Add Game Content
  2. Logout
```

#### Admin Features:

**1. Add Game Content**
```
Menu:
  1. Word Matching
  2. Sentence Matching
  3. Picture Matching

Enter choice (1-3): 1

Word 1: apple
Word 2/Definition: A red or green fruit

✓ Game item added successfully!
```

---

## 🎯 Key Features

### Password Security
- Passwords are hidden during input (shown as ***)
- Works on both Windows and Linux

### Role-Based Menus
- **Student**: Learning features (lessons, exercises, games)
- **Teacher**: Feedback and student communication
- **Admin**: Content management

### Auto-Detection
- Server returns role during login
- Client automatically shows appropriate menu
- No manual role selection needed

### Clean Interface
- Clear screen between menus
- Formatted headers
- Success/Error messages with ✓/✗ symbols
- "Press Enter to continue" prompts

---

## 💡 Tips

1. **Quick Testing**: Use default accounts (admin/admin123 or teacher1/teacher123)
2. **Multi-user**: Run multiple console clients in different terminals
3. **Logging**: Check `logs/console_client.log` for detailed logs
4. **Navigation**: Enter 0 or use Back option to return to previous menu

---

## 🐛 Troubleshooting

**Cannot connect to server:**
- Ensure server is running: `./server`
- Check server is on 0.0.0.0:8080
- Verify no firewall blocking

**Login fails:**
- Double-check username/password
- Try registering a new account first
- Check server logs: `logs/server.log`

**Menu doesn't show:**
- Ensure terminal supports clear screen
- Try resizing terminal window
- Check console_client.log for errors

---

## 📊 Comparison

| Feature | Console Client | Qt GUI Client |
|---------|---------------|---------------|
| Requires Qt | ❌ No | ✅ Yes |
| File Size | ~140 KB | ~212 KB |
| GUI | Text-based | Graphical |
| Role Selection | Auto-detected | Manual |
| Platform | Any terminal | Desktop only |
| Lightweight | ✅ Yes | ❌ No |
| All Features | ✅ Yes | ✅ Yes |

---

## 🔄 Workflow Example

### Complete Student Workflow:
```
1. Start console_client
2. Connect to 127.0.0.1:8080
3. Login as student1
   → Automatically enters Student menu
4. Set level to Intermediate
5. Browse lessons → View lesson
6. Submit quiz → Get score
7. View score & feedback
8. Logout
```

### Complete Teacher Workflow:
```
1. Start console_client
2. Connect to server
3. Login as teacher1
   → Automatically enters Teacher menu
4. Provide feedback to student1
5. Chat with student1
6. Logout
```

---

## 📝 Notes

- Terminal must support ANSI escape codes for best experience
- On Windows, use Windows Terminal or modern console
- On Linux/macOS, any standard terminal works
- Logs are saved to `logs/console_client.log`

---

**Enjoy using the Console Client!** 🚀

For GUI version, use `./client` instead.

