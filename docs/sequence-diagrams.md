# Sequence Diagrams

This document contains sequence diagrams for the main features of the learning application.

## 1. User Registration

```plantuml
@startuml
actor User
participant "UI/ConsoleClient" as UI
participant Client
participant "TCP Socket" as Socket
participant Server
participant ClientHandler
participant Database

User -> UI: Enter registration details\n(username, password, role)
UI -> UI: Validate password confirmation
UI -> Client: registerUser(username, password, role)
Client -> Client: createRegisterRequest(username, password, role)
Client -> Socket: sendMessageSync(REGISTER_REQUEST)
Socket -> Server: TCP transmission
Server -> ClientHandler: handleMessage()
ClientHandler -> ClientHandler: handleRegisterRequest()
ClientHandler -> Database: userExists(username)
Database --> ClientHandler: false
ClientHandler -> Database: hashPassword(password)
Database --> ClientHandler: passwordHash
ClientHandler -> Database: createUser(username, passwordHash, role)
Database -> Database: Store user data\n(username, passwordHash, role,\nlevel=BEGINNER, score=0)
Database --> ClientHandler: true
ClientHandler --> Server: REGISTER_SUCCESS
Server --> Socket: TCP response
Socket --> Client: REGISTER_SUCCESS
Client --> UI: true
UI --> User: Show success message\n"Registration successful!"

alt Registration Failed - User Exists
    Database --> ClientHandler: false (user exists)
    ClientHandler --> Client: REGISTER_FAILED\n"Username already exists"
    Client --> UI: false
    UI --> User: Show error message
end

alt Registration Failed - Database Error
    Database --> ClientHandler: false
    ClientHandler --> Client: ERROR\n"Failed to create user"
    Client --> UI: false
    UI --> User: Show error message
end


alt Socket Connection Error
    Client -> Socket: sendMessageSync(REGISTER_REQUEST)
    Socket --> Client: SocketConnectionError
    Client --> UI: false
    UI --> User: Show error message\n"Socket connection error. Please try again."
end

alt Server Lost Database Connection
    ClientHandler -> Database: createUser(username, passwordHash, role)
    Database --> ClientHandler: ConnectionError
    ClientHandler --> Server: SERVER_ERROR
    Server --> Socket: TCP response (SERVER_ERROR)
    Socket --> Client: SERVER_ERROR
    Client --> UI: false
    UI --> User: Show error message\n"Server error. Please try again later."
end

@enduml

```

## 2. User Login

```plantuml
@startuml
actor User
participant "UI/ConsoleClient" as UI
participant Client
participant "TCP Socket" as Socket
participant Server
participant ClientHandler
participant Database

User -> UI: Enter credentials\n(username, password)
UI -> Client: login(username, password, userData)
Client -> Client: createLoginRequest(username, password)
Client -> Socket: sendMessageSync(LOGIN_REQUEST)
Socket -> Server: TCP transmission
Server -> ClientHandler: handleMessage()
ClientHandler -> ClientHandler: handleLoginRequest()
ClientHandler -> Database: hashPassword(password)
Database --> ClientHandler: passwordHash
ClientHandler -> Database: authenticateUser(username, passwordHash)
Database -> Database: Compare passwordHash
Database --> ClientHandler: true (authenticated)
ClientHandler -> Database: getUserData(username)
Database --> ClientHandler: UserData(role, level, score)
ClientHandler -> ClientHandler: Set authenticated = true\nStore username, role, level
ClientHandler -> Database: createSession(username, socket)
Database -> Database: Create SessionData\n(socket, username, state=AUTHENTICATED,\nlastActivity, role)
Database --> ClientHandler: true
ClientHandler -> ClientHandler: Format response:\nrole|level|score
ClientHandler --> Server: LOGIN_SUCCESS\n"role|level|score"
Server --> Socket: TCP response
Socket --> Client: LOGIN_SUCCESS\n"role|level|score"
Client -> Client: Parse response\nExtract role, level, score
Client --> UI: true + UserData
UI -> UI: Set authenticated = true\nStore currentUser
UI --> User: Show success message\n"Logged in successfully!\nRole: [role]\nLevel: [level]\nScore: [score]"

alt Login Failed - Invalid Credentials
    Database --> ClientHandler: false (auth failed)
    ClientHandler --> Client: LOGIN_FAILED\n"Invalid username or password"
    Client --> UI: false
    UI --> User: Show error message
end


alt Socket Connection Error
    Client -> Socket: sendMessageSync(LOGIN_REQUEST)
    Socket --> Client: SocketConnectionError
    Client --> UI: false
    UI --> User: Show error message\n"Socket connection error. Please try again."
end

alt Server Lost Database Connection
    ClientHandler -> Database: authenticateUser(username, passwordHash)
    Database --> ClientHandler: ConnectionError
    ClientHandler --> Server: SERVER_ERROR
    Server --> Socket: TCP response (SERVER_ERROR)
    Socket --> Client: SERVER_ERROR
    Client --> UI: false
    UI --> User: Show error message\n"Server error. Please try again later."
end

@enduml
```

## 3. Do Quiz (Submit Quiz)

```plantuml
@startuml
actor Student
participant "UI/ConsoleClient" as UI
participant Client
participant "TCP Socket" as Socket
participant Server
participant ClientHandler
participant Database

Student -> UI: Enter quiz details\n(quizId, answers)
UI -> UI: Check authenticated
UI -> Client: submitQuiz(quizId, answers)
Client -> Client: Create payload:\nquizId|answers
Client -> Socket: sendMessageSync(SUBMIT_QUIZ_REQUEST)
Socket -> Server: TCP transmission
Server -> ClientHandler: handleMessage()
ClientHandler -> ClientHandler: Check authenticated
ClientHandler -> ClientHandler: handleSubmitQuizRequest()
ClientHandler -> Database: saveQuizSubmission(username, quizId, answers)
Database -> Database: Store quiz submission data
Database --> Server: true
Server --> Socket: TCP response
Socket --> Client: SUBMIT_QUIZ_SUCCESS
Client --> UI: true
UI --> Student: Show success message\n"Quiz submitted successfully!"

alt Not Authenticated
    ClientHandler -> ClientHandler: authenticated = false
    ClientHandler --> Client: ERROR\n"Authentication required"
    Client --> UI: false
    UI --> Student: Show error message\n"Please login first"
end

alt Invalid Format
    ClientHandler -> ClientHandler: Parse fails
    ClientHandler --> Client: ERROR\n"Invalid quiz submission"
    Client --> UI: false
    UI --> Student: Show error message
end

alt Socket Connection Error
    Client -> Socket: sendMessageSync(SUBMIT_QUIZ_REQUEST)
    Socket --> Client: SocketConnectionError
    Client --> UI: false
    UI --> Student: Show error message\n"Socket connection error. Please try again."
end

alt Server Lost Database Connection
    ClientHandler -> Database: saveQuizSubmission(username, quizId, answers)
    Database --> ClientHandler: ConnectionError
    ClientHandler --> Server: SERVER_ERROR
    Server --> Socket: TCP response (SERVER_ERROR)
    Socket --> Client: SERVER_ERROR
    Client --> UI: false
    UI --> Student: Show error message\n"Server error. Please try again later."
end

@enduml

```

## 4. View Lesson List

```plantuml
@startuml
actor Student
participant "UI/ConsoleClient" as UI
participant Client
participant "TCP Socket" as Socket
participant Server
participant ClientHandler
participant Database

Student -> UI: Click "Browse Lessons" or\n"Refresh Lessons"
UI -> UI: Check authenticated
UI -> Client: getLessonList()
Client -> Socket: sendMessageSync(GET_LESSON_LIST_REQUEST)
Socket -> Server: TCP transmission
Server -> ClientHandler: handleMessage()
ClientHandler -> ClientHandler: Check authenticated
ClientHandler -> ClientHandler: handleGetLessonListRequest()
ClientHandler -> Database: getLessonList(level)
Database -> Database: Get level string\n(BEGINNER/INTERMEDIATE/ADVANCED)
Database -> Database: Lookup lessons by level\nfrom lessons_ map
Database --> ClientHandler: vector<string> lessons\n["lesson_b1:Greetings",\n"lesson_b2:Numbers",\n"lesson_b3:Family"]
ClientHandler -> ClientHandler: Format response:\nJoin with semicolons\n"lesson_b1:Greetings;lesson_b2:Numbers;..."
ClientHandler --> Server: GET_LESSON_LIST_RESPONSE\n"lesson1;lesson2;lesson3"
Server --> Socket: TCP response
Socket --> Client: GET_LESSON_LIST_RESPONSE\n"lesson1;lesson2;lesson3"
Client -> Client: Parse payload:\nSplit by semicolon
Client --> UI: vector<string> lessons
UI -> UI: Display lessons in list/widget\n(with title, description, status)
UI --> Student: Show lesson list

alt Not Authenticated
    ClientHandler -> ClientHandler: authenticated = false
    ClientHandler --> Client: ERROR\n"Authentication required"
    Client --> UI: empty vector
    UI --> Student: Show error message\n"Please login first"
end

alt No Lessons Available
    Database --> ClientHandler: empty vector
    ClientHandler --> Client: GET_LESSON_LIST_RESPONSE\n"" (empty)
    Client --> UI: empty vector
    UI --> Student: Show message\n"No lessons available"
end

@enduml
```

## 5. View Lesson Video (View Lesson Content)

```plantuml
@startuml
actor Student
participant "UI/ConsoleClient" as UI
participant Client
participant "TCP Socket" as Socket
participant Server
participant ClientHandler
participant Database

Student -> UI: Select lesson from list\nClick "View Lesson"
UI -> UI: Extract lessonId\n(from "id:title" format)
UI -> Client: getLessonContent(lessonId)
Client -> Socket: sendMessageSync(GET_LESSON_CONTENT_REQUEST,\nlessonId)
Socket -> Server: TCP transmission
Server -> ClientHandler: handleMessage()
ClientHandler -> ClientHandler: Check authenticated
ClientHandler -> ClientHandler: handleGetLessonContentRequest()
ClientHandler -> ClientHandler: Trim lessonId from payload
ClientHandler -> Database: getLessonContent(lessonId)
Database -> Database: Fetch lesson content\n(In real implementation:\nretrieve video URL, audio URL,\ntext content, etc.)
Database --> ClientHandler: "Content for lesson: [lessonId]\nVideo: video_url\nAudio: audio_url\nText: lesson_text"
ClientHandler --> Server: GET_LESSON_CONTENT_RESPONSE\ncontent string
Server --> Socket: TCP response
Socket --> Client: GET_LESSON_CONTENT_RESPONSE\ncontent string
Client --> UI: content string
UI -> UI: Parse content:\n- Extract video URL\n- Extract audio URL\n- Extract text content\n- Extract vocabulary/grammar
UI -> UI: Display in tabbed interface:\n- Video tab (embedded player)\n- Audio tab (audio player)\n- Text tab (transcript)\n- Vocabulary/Grammar tab
UI --> Student: Show lesson content\nwith video player ready

alt Not Authenticated
    ClientHandler -> ClientHandler: authenticated = false
    ClientHandler --> Client: ERROR\n"Authentication required"
    Client --> UI: empty string
    UI --> Student: Show error message\n"Please login first"
end

note right of UI
  In GUI version (UI.cpp):
  - lessonContentText displays content
  - Video/audio players would be embedded
  
  In Console version (ConsoleClient):
  - Display text content
  - Show URLs for video/audio
end note

@enduml
```

## 6. Play Word Matching Game

```plantuml
@startuml
actor Student
participant "UI/ConsoleClient" as UI
participant Client
participant "TCP Socket" as Socket
participant Server
participant ClientHandler
participant Database

== Game Start ==
Student -> UI: Select "Word Matching" game\nClick "Start Game"
UI -> UI: Check authenticated
UI -> Client: startGame("Word Matching")
Client -> Socket: sendMessageSync(GAME_START_REQUEST,\n"Word Matching")
Socket -> Server: TCP transmission
Server -> ClientHandler: handleMessage()
ClientHandler -> ClientHandler: Check authenticated
ClientHandler -> ClientHandler: handleGameStartRequest()
ClientHandler -> ClientHandler: Trim gameType from payload
ClientHandler -> Database: getGameItems("Word Matching")
Database -> Database: Lookup game items\nby game type
Database --> ClientHandler: vector<string>\n["word1=definition1",\n"word2=definition2",...]
ClientHandler -> ClientHandler: Create game session\nFormat response:\n"game_session_id_123|item1;item2;..."
ClientHandler --> Server: GAME_START_RESPONSE\n"session_id|items"
Server --> Socket: TCP response
Socket --> Client: GAME_START_RESPONSE\n"session_id|items"
Client --> UI: gameData string
UI -> UI: Parse game data:\n- Extract session ID\n- Extract word pairs\n- Setup game UI\n(two columns for matching)
UI --> Student: Display game interface\nwith word matching pairs

== Game Move ==
Student -> UI: Match words\n(e.g., "match_word1_word2")
UI -> Client: sendGameMove(moveData, response)
Client -> Socket: sendMessageSync(GAME_MOVE_REQUEST,\nmoveData)
Socket -> Server: TCP transmission
Server -> ClientHandler: handleMessage()
ClientHandler -> ClientHandler: Check authenticated
ClientHandler -> ClientHandler: handleGameMoveRequest()
ClientHandler -> ClientHandler: Validate move\n(Simple validation:\ncheck format, check words exist)
ClientHandler -> ClientHandler: Calculate score\n(e.g., +10 for correct match)
ClientHandler -> ClientHandler: Format result:\n"move_accepted|score:10"
ClientHandler --> Server: GAME_MOVE_RESPONSE\n"move_accepted|score:10"
Server --> Socket: TCP response
Socket --> Client: GAME_MOVE_RESPONSE\n"move_accepted|score:10"
Client --> UI: response string + true
UI -> UI: Parse response:\n- Extract move status\n- Extract score earned
UI -> UI: Update game display:\n- Mark matched pairs\n- Update score display
UI --> Student: Show result\n"Correct match! +10 points"

== Game End ==
Student -> UI: Complete all matches or quit
UI -> UI: Calculate final score\nShow completion message
UI --> Student: Show final score\nand game summary

alt Not Authenticated
    ClientHandler -> ClientHandler: authenticated = false
    ClientHandler --> Client: ERROR\n"Authentication required"
    Client --> UI: empty string / false
    UI --> Student: Show error message\n"Please login first"
end

alt Invalid Move
    ClientHandler -> ClientHandler: Move validation fails
    ClientHandler --> Client: GAME_MOVE_RESPONSE\n"move_rejected|invalid"
    Client --> UI: response + true
    UI --> Student: Show message\n"Invalid move, try again"
end

note right of Database
  Game items stored by type:
  - Word Matching: word=definition
  - Sentence Matching: sentence1=sentence2
  - Picture Matching: image_path=text
end note

@enduml
```

## Architecture Overview

### Components
- **UI/ConsoleClient**: User interface layer (GUI or Console)
- **Client**: Client-side business logic and message handling
- **TCP Socket**: Network communication layer
- **Server**: Server-side request routing
- **ClientHandler**: Handles client requests and business logic
- **Database**: Data persistence layer (in-memory for this implementation)

### Message Flow
1. User interacts with UI
2. UI calls Client methods
3. Client creates and sends messages via TCP Socket
4. Server receives and routes to ClientHandler
5. ClientHandler processes request and interacts with Database
6. Response flows back through the same chain

### Authentication
- All operations (except register/login) require authentication
- Session is created on successful login
- Session data includes: socket, username, role, last activity
- ClientHandler maintains authenticated state per connection

### Data Structures
- **UserData**: username, passwordHash, role, level, score
- **SessionData**: socket, username, state, role, lastActivity
- **Message**: header (type, payloadLength, timestamp, version) + payload

## Notes

1. **Security**: Passwords are hashed before storage using Database::hashPassword()
2. **Session Management**: Sessions are created on login and removed on logout
3. **Proficiency Levels**: BEGINNER, INTERMEDIATE, ADVANCED
4. **User Roles**: STUDENT, TEACHER, ADMIN
5. **Message Types**: Each operation has dedicated request/response message types
6. **Error Handling**: Each operation has error paths for authentication, validation, and database errors

## PlantUML Rendering

To render these diagrams:
1. Use PlantUML tool or online editor (http://www.plantuml.com/plantuml/)
2. Copy each diagram block (including @startuml and @enduml tags)
3. Paste into PlantUML editor to generate the visual diagram

Alternatively, use VSCode with PlantUML extension or IntelliJ IDEA with PlantUML plugin.

