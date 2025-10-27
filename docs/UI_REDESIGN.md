# UI Redesign - Screen-Based Navigation

## Overview

The UI has been redesigned from a simple tab-based interface to a comprehensive screen-based navigation system that matches the specifications in `screen-setting.md`.

## Architecture Changes

### Before (Old Design)
- 7 tabs in a single window
- All features accessible at once
- Simple navigation

### After (New Design)
- 16 distinct screens with stack-based navigation
- Role-based screen visibility (Student/Teacher/Admin)
- Professional dashboard and navigation flow
- Matches the screen specifications exactly

---

## Screen Structure

### Student Screens (S-1 to S-10)

**S-1: Login/Registration Screen**
- Username/Email + Password fields
- Registration form with email, password confirmation, role selection
- Toggle between login and registration
- Connection status indicator

**S-2: Initial Setup/Profile**
- Proficiency level selection (Beginner/Intermediate/Advanced)
- Content file location settings
- Save and continue to dashboard

**S-3: Main Dashboard** (Navigation Hub)
- Welcome message with user info
- Current level and score display
- 6 large navigation buttons:
  - 📚 Lessons (Blue)
  - ✏️ Exercises (Orange)
  - 🎮 Games (Green)
  - 💬 Chat (Purple)
  - ⚙️ Profile & Settings (Gray)
  - 🚪 Logout (Red)
- Recent activity feed

**S-4: Lesson List**
- Filter dropdowns (Topic, Level)
- Scrollable list of lessons with:
  - Lesson title
  - Brief description
  - Status (New/In Progress/Completed)
- Click to open S-5
- Back to Dashboard button

**S-5: Lesson View**
- Lesson title
- Tabbed interface:
  - Video tab (embedded player)
  - Audio tab (audio player)
  - Text tab (lesson transcript)
  - Vocabulary/Grammar tab
- "Go to Exercises" button
- Back to Lesson List button

**S-6: Exercise/Test Screen**
- Question display area
- Dynamic answer input based on type:
  - Multiple Choice (radio buttons/checkboxes)
  - Fill-in-the-Blanks (text fields)
  - Rearrangement (drag-drop or clickable blocks)
  - Writing (large text area)
- Submit/Next buttons
- Optional timer display
- Back to Lessons button

**S-7: Speaking Exercise**
- Topic/scenario prompt display
- Recording controls:
  - Start Recording button
  - Stop Recording button
  - Playback button
- Submit Recording button
- Recording status indicator

**S-8: Score & Feedback**
- Numerical score display
- Completion status
- System feedback (automated)
- Teacher feedback (from teachers)
- "View More Feedback" button
- Back button

**S-9: Games**
- Game type selector dropdown:
  - Word Matching
  - Sentence Matching
  - Picture Matching
- Interactive game area:
  - Two columns for matching
  - Drag-and-drop or click to pair
- Start/Reset/Submit buttons
- Score display
- Back to Dashboard button

**S-10: Communication**
- Contact list (Users/Teachers)
- Chat window:
  - Message history
  - Text input field
  - Send button
- Voice call interface:
  - Initiate Call button
  - Mute button
  - End Call button
- Back to Dashboard button

---

### Teacher Screens (T-1 to T-3)

**Teacher Dashboard**
- Welcome message
- Navigation to:
  - Feedback Inbox
  - Student Chat
  - Profile

**T-1: Feedback Inbox/Review**
- List of student submissions
- Filters (by Lesson/Type)
- Click to open T-2

**T-2: Provide Feedback**
- Student work display
- Text feedback input area
- Audio feedback recording option
- Submit Feedback button

**T-3: Communication**
- Student list
- Chat window (same as S-10 but filtered for students)
- Voice call interface

---

### Admin Screens (A-1)

**Admin Dashboard**
- Welcome message
- Navigation to:
  - Game Content Management
  - System Logs
  - Profile

**A-1: Game Content Management**
- Game type selector (Word/Sentence/Picture Matching)
- Dynamic form based on game type:
  - **Word Matching**:
    - Word 1 field
    - Word 2/Definition field
  - **Sentence Matching**:
    - Sentence 1 field
    - Sentence 2/Completion field
  - **Picture Matching**:
    - Image upload button
    - Corresponding text field
- Add Item button
- List of existing items

---

## Navigation Flow

```
Login Screen (S-1)
    ↓
Profile Setup (S-2) [First time only]
    ↓
Main Dashboard (S-3)
    ├→ Lessons (S-4) → Lesson View (S-5) → Exercises (S-6)
    ├→ Exercises (S-6) → Speaking (S-7) → Score & Feedback (S-8)
    ├→ Games (S-9)
    ├→ Chat (S-10)
    ├→ Profile (S-2)
    └→ Logout → Login (S-1)

Teacher Flow:
Login → Teacher Dashboard → Feedback Inbox (T-1) → Provide Feedback (T-2)
                          └→ Chat (T-3)

Admin Flow:
Login → Admin Dashboard → Game Content (A-1)
```

---

## Implementation Details

### Key Classes and Structures

**MainWindow** - Main application window
- Uses `QStackedWidget` for screen management
- Each screen is a separate `QWidget`
- Navigation methods: `navigateToScreen(int index)`

**Screen Indices** (enum)
```cpp
SCREEN_LOGIN = 0
SCREEN_PROFILE = 1
SCREEN_DASHBOARD = 2
SCREEN_LESSON_LIST = 3
SCREEN_LESSON_VIEW = 4
SCREEN_EXERCISE = 5
SCREEN_SPEAKING = 6
SCREEN_SCORE_FEEDBACK = 7
SCREEN_GAMES = 8
SCREEN_CHAT = 9
SCREEN_TEACHER_DASHBOARD = 10
SCREEN_FEEDBACK_INBOX = 11
SCREEN_PROVIDE_FEEDBACK = 12
SCREEN_TEACHER_CHAT = 13
SCREEN_ADMIN_DASHBOARD = 14
SCREEN_GAME_CONTENT = 15
```

### Role-Based Visibility

```cpp
void showStudentScreens() {
    // Show S-1 to S-10
    // Hide teacher and admin screens
}

void showTeacherScreens() {
    // Show teacher dashboard and T-1 to T-3
    // Hide admin screens
}

void showAdminScreens() {
    // Show admin dashboard and A-1
}
```

---

## Benefits of New Design

1. **Better UX**: Clear navigation paths and focused screens
2. **Role Separation**: Each role sees only relevant screens
3. **Scalability**: Easy to add new screens and features
4. **Professional**: Matches modern application design patterns
5. **Spec Compliance**: Exactly matches the screen-setting.md requirements

---

## Migration Notes

### For Users:
- No more tab-based interface
- Use navigation buttons to move between screens
- Dashboard is the main hub
- Each feature has its own dedicated screen

### For Developers:
- Each screen is implemented in its own `setup*Screen()` method
- Signal/slot connections for navigation
- Shared state in `MainWindow` class
- Easy to modify individual screens without affecting others

---

## Testing the New UI

1. **Login Flow**:
   - Start application
   - See S-1 (Login screen)
   - Register a new account or login
   - Complete profile setup (S-2)
   - Land on dashboard (S-3)

2. **Student Flow**:
   - From dashboard, click "Lessons"
   - Browse lesson list (S-4)
   - Click a lesson to view (S-5)
   - Go to exercises (S-6)
   - Complete and see scores (S-8)

3. **Teacher Flow**:
   - Login as teacher
   - See teacher dashboard
   - Access feedback inbox
   - Provide feedback to students

4. **Admin Flow**:
   - Login as admin
   - See admin dashboard
   - Manage game content

---

## Future Enhancements

- Add transition animations between screens
- Implement breadcrumb navigation
- Add screen history/back stack
- Keyboard shortcuts for navigation
- Responsive layout for different screen sizes

---

This redesign provides a professional, role-based user interface that exactly matches the requirements and provides a much better user experience than the simple tab-based interface.

