# 🚀 How to Run the Server

## ✅ Server is Ready!

The server has been successfully built and is ready to run.

---

## Start the Server

```bash
cd /mnt/c/Users/tra01/OneDrive/Desktop/socker/project
./server
```

You should see:

```
=== English Learning Platform Server ===
Initializing...
Setting up logging...
Loading configuration...
Initializing database...
Creating server instance...
Initializing server on 0.0.0.0:8080...

✓ Server listening on 0.0.0.0:8080
✓ Database initialized with default accounts:
  - admin / admin123 (Admin)
  - teacher1 / teacher123 (Teacher)

Press Ctrl+C to stop the server
```

The server is now running and waiting for client connections!

---

## Start the Client (in a new terminal)

```bash
cd /mnt/c/Users/tra01/OneDrive/Desktop/socker/project
./build/bin/client
```

The Qt GUI window will open.

---

## Quick Test

1. **In the client GUI:**
   - Go to "Connection" tab
   - Server: `127.0.0.1`
   - Port: `8080`
   - Click "Connect"

2. **Register an account:**
   - Go to "Authentication" tab
   - Username: `student1`
   - Password: `password123`
   - Role: Student
   - Click "Register"

3. **Login:**
   - Click "Login" with the same credentials

4. **Explore the features!**

---

## Default Accounts

| Username | Password | Role |
|----------|----------|------|
| admin | admin123 | Admin |
| teacher1 | teacher123 | Teacher |

---

## Stop the Server

Press `Ctrl+C` in the terminal where the server is running.

---

## Bugs Fixed

During testing, two critical deadlock bugs were found and fixed:

1. **Logger deadlock**: The `initialize()` method was calling `log()` while holding a mutex, and `log()` tried to acquire the same mutex.

2. **Database deadlock**: The `initialize()` method was calling `createUser()` while holding a mutex, and `createUser()` tried to acquire the same mutex.

Both have been resolved and the system now runs smoothly!

---

## Logs

Check the logs for debugging:
- Server: `logs/server.log`
- Client: `logs/client.log`

---

Enjoy your English Learning Platform! 📚✨

