#!/bin/bash

# Build script for Linux/macOS

echo "=== English Learning Platform Build Script ==="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Create build directory
if [ -d "build" ]; then
    echo -e "${YELLOW}Cleaning existing build directory...${NC}"
    rm -rf build
fi

echo -e "${GREEN}Creating build directory...${NC}"
mkdir -p build
cd build

# Run CMake
echo ""
echo -e "${GREEN}Running CMake configuration...${NC}"
cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo -e "${RED}CMake configuration failed!${NC}"
    exit 1
fi

# Build
echo ""
echo -e "${GREEN}Building project...${NC}"
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

# Create necessary directories
echo ""
echo -e "${GREEN}Creating runtime directories...${NC}"
cd ..
mkdir -p logs
mkdir -p data

# Copy executables to root for easier access (optional)
if [ -f "build/bin/server" ]; then
    cp build/bin/server ./
    echo -e "${GREEN}Server executable: ./server${NC}"
fi

if [ -f "build/bin/client" ]; then
    cp build/bin/client ./
    echo -e "${GREEN}Client executable: ./client${NC}"
fi

echo ""
echo -e "${GREEN}=== Build completed successfully! ===${NC}"
echo ""
echo "To run the server:"
echo "  ./server"
echo ""
echo "To run the client (if Qt is installed):"
echo "  ./client"
echo ""

