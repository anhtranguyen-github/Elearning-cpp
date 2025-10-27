#!/bin/bash

# --- Thư mục config ---
mkdir -p config
touch config/client_config.json
touch config/server_config.json

# --- Thư mục include ---
mkdir -p include
touch include/common.hpp
touch include/message_structs.hpp

# --- Thư mục src/server ---
mkdir -p src/server
touch src/server/main.cpp
touch src/server/Server.cpp
touch src/server/Server.hpp
touch src/server/ClientHandler.cpp
touch src/server/ClientHandler.hpp

# --- Thư mục src/client ---
mkdir -p src/client
touch src/client/main.cpp
touch src/client/Client.cpp
touch src/client/Client.hpp
touch src/client/UI.cpp
touch src/client/UI.hpp

# --- Thư mục src/protocol ---
mkdir -p src/protocol
touch src/protocol/Protocol.cpp
touch src/protocol/Protocol.hpp
touch src/protocol/Network.cpp
touch src/protocol/Network.hpp

# --- Thư mục src/utils ---
mkdir -p src/utils
touch src/utils/Logger.cpp
touch src/utils/Logger.hpp
touch src/utils/Parser.cpp
touch src/utils/Parser.hpp

# --- Thư mục src/db ---
mkdir -p src/db
touch src/db/Database.cpp
touch src/db/Database.hpp

# --- Thư mục logs ---
mkdir -p logs
touch logs/server.log
touch logs/client.log

# --- Thư mục test ---
mkdir -p test
touch test/test_message_format.cpp
touch test/test_network_connection.cpp

# --- File gốc ở root (đảm bảo tồn tại) ---
touch CMakeLists.txt
touch Makefile
touch README.md
touch build_project.sh

echo "✅ Project structure created successfully inside $(pwd)"
