**Use Case List decomposed by Actor and strictly adhering to the listed functionalities and requirements.**

---

## I. Actor: Student (User)

| **ID** | **Level 1 Use Case** | **Breakdown (Level 2 Use Case)** |
| --- | --- | --- |
| 1.0 | Account Management | 1.1 Register Account |
|  |  | 1.2 Log In to Account |
| 2.0 | Study Setup | 2.1 Select English Proficiency Level (beginner, intermediate, advanced) |
| 3.0 | Content Access | 3.1 View Lesson List (by topic and level) |
| 4.0 | Lesson Study | 4.1 View Lesson Video |
|  |  | 4.2 Listen to Lesson Audio |
|  |  | 4.3 Read Lesson Text |
|  |  | 4.4 View Vocabulary and Grammar |
| 5.0 | Complete Exercises & Tests | 5.1 Take Multiple Choice Quiz |
|  |  | 5.2 Take Fill-in-the-Blanks Test |
|  |  | 5.3 Take Sentence Rearrangement Test |
|  |  | 5.4 Do Sentence Rewriting Exercise |
|  |  | 5.5 Do Paragraph Writing Exercise |
|  |  | 5.6 Do Topic-based Speaking Exercise |
| 6.0 | Participate in Games | 6.1 Play Word Matching Game |
|  |  | 6.2 Play Sentence Matching Game |
|  |  | 6.3 Play Picture Matching Game |
| 7.0 | Assessment & Resources | 7.1 Receive Score |
|  |  | 7.2 Receive Feedback from system/teacher |
|  |  | 7.3 Select Content File Location (Select addresses of machines hosting these files) |
| 8.0 | Communication | 8.1 Chat with Other Users |
|  |  | 8.2 Chat with Teacher |
|  |  | 8.3 Conduct Voice Call with Other Users |
|  |  | 8.4 Conduct Voice Call with Teacher |

---

## II. Actor: Teacher

| **ID** | **Level 1 Use Case** | **Breakdown (Level 2 Use Case)** |
| --- | --- | --- |
| 1.0 | Assessment & Feedback | 1.1 Provide Feedback for student exercises/tests (Teacher feedback) |
| 2.0 | Communication | 2.1 Chat with Student |
|  |  | 2.2 Conduct Voice Call with Student |

---

## III. Actor: Admin (Administrator) / System

| **ID** | **Level 1 Use Case** | **Breakdown (Level 2 Use Case)** |
| --- | --- | --- |
| 1.0 | Game Content Management | 1.1 Add Item for Word Matching Game |
|  |  | 1.2 Add Item for Sentence Matching Game |
|  |  | 1.3 Add Item for Picture Matching Game |
| 2.0 | System Operation (Technical/Administrative Requirements) | 2.1 Log incoming and outgoing messages (server side) |
|  |  | 2.2 Provide Socket Connection (Connect client server using socket) |