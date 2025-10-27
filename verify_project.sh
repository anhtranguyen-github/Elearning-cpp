#!/bin/bash

# Project verification script

echo "========================================"
echo "  English Learning Platform"
echo "  Project Verification Script"
echo "========================================"
echo ""

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to check file exists
check_file() {
    if [ -f "$1" ]; then
        echo -e "${GREEN}✓${NC} $1"
        return 0
    else
        echo -e "${RED}✗${NC} $1 (missing)"
        return 1
    fi
}

# Function to check directory exists
check_dir() {
    if [ -d "$1" ]; then
        echo -e "${GREEN}✓${NC} $1/"
        return 0
    else
        echo -e "${RED}✗${NC} $1/ (missing)"
        return 1
    fi
}

echo "Checking project structure..."
echo ""

# Check directories
echo "Directories:"
check_dir "config"
check_dir "include"
check_dir "src"
check_dir "src/server"
check_dir "src/client"
check_dir "src/protocol"
check_dir "src/utils"
check_dir "src/db"
check_dir "logs"
check_dir "test"
check_dir "docs"

echo ""
echo "Core Headers:"
check_file "include/common.hpp"
check_file "include/message_structs.hpp"

echo ""
echo "Utility Layer:"
check_file "src/utils/Logger.hpp"
check_file "src/utils/Logger.cpp"
check_file "src/utils/Parser.hpp"
check_file "src/utils/Parser.cpp"

echo ""
echo "Protocol Layer:"
check_file "src/protocol/Protocol.hpp"
check_file "src/protocol/Protocol.cpp"
check_file "src/protocol/Network.hpp"
check_file "src/protocol/Network.cpp"

echo ""
echo "Database Layer:"
check_file "src/db/Database.hpp"
check_file "src/db/Database.cpp"

echo ""
echo "Server:"
check_file "src/server/Server.hpp"
check_file "src/server/Server.cpp"
check_file "src/server/ClientHandler.hpp"
check_file "src/server/ClientHandler.cpp"
check_file "src/server/main.cpp"

echo ""
echo "Client:"
check_file "src/client/Client.hpp"
check_file "src/client/Client.cpp"
check_file "src/client/UI.hpp"
check_file "src/client/UI.cpp"
check_file "src/client/main.cpp"

echo ""
echo "Tests:"
check_file "test/test_message_format.cpp"
check_file "test/test_network_connection.cpp"

echo ""
echo "Configuration:"
check_file "config/server_config.json"
check_file "config/client_config.json"

echo ""
echo "Build System:"
check_file "CMakeLists.txt"
check_file "Makefile"
check_file "build_project.sh"

echo ""
echo "Documentation:"
check_file "README.md"
check_file "QUICKSTART.md"
check_file "PROJECT_SUMMARY.md"

echo ""
echo "========================================"

# Count files
cpp_count=$(find src test -name "*.cpp" | wc -l)
hpp_count=$(find src include -name "*.hpp" | wc -l)

echo ""
echo "Statistics:"
echo "  C++ implementation files: $cpp_count"
echo "  C++ header files: $hpp_count"
echo "  Total source files: $((cpp_count + hpp_count))"

echo ""
echo "========================================"
echo ""

# Check if build was attempted
if [ -d "build" ]; then
    echo -e "${YELLOW}Build directory exists${NC}"
    
    if [ -f "build/bin/server" ] || [ -f "build/bin/server.exe" ]; then
        echo -e "${GREEN}✓ Server executable found${NC}"
    else
        echo -e "${YELLOW}! Server not built yet${NC}"
    fi
    
    if [ -f "build/bin/client" ] || [ -f "build/bin/client.exe" ]; then
        echo -e "${GREEN}✓ Client executable found${NC}"
    else
        echo -e "${YELLOW}! Client not built yet (may require Qt5)${NC}"
    fi
else
    echo -e "${YELLOW}! Project not built yet${NC}"
    echo "  Run: ./build_project.sh"
fi

echo ""
echo "========================================"
echo -e "${GREEN}Project structure verification complete!${NC}"
echo ""
echo "Next steps:"
echo "  1. Build the project: ./build_project.sh"
echo "  2. Run the server: ./build/bin/server"
echo "  3. Run the client: ./build/bin/client"
echo "  4. Read QUICKSTART.md for quick setup"
echo "  5. Read README.md for full documentation"
echo ""

