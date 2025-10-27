# Makefile for quick build commands

.PHONY: all build clean run-server run-client help

all: build

# Build project using CMake
build:
	@echo "Building project..."
	@mkdir -p build
	@cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j$$(nproc)
	@mkdir -p logs data
	@echo "Build complete!"

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf build
	@rm -f server client
	@echo "Clean complete!"

# Run server
run-server: build
	@echo "Starting server..."
	@./build/bin/server

# Run client
run-client: build
	@echo "Starting client..."
	@./build/bin/client

# Full clean (including logs)
cleanall: clean
	@echo "Cleaning logs and data..."
	@rm -rf logs/*.log
	@echo "Full clean complete!"

# Help
help:
	@echo "Available targets:"
	@echo "  make build      - Build the project"
	@echo "  make clean      - Clean build artifacts"
	@echo "  make cleanall   - Clean everything including logs"
	@echo "  make run-server - Build and run server"
	@echo "  make run-client - Build and run client"
	@echo "  make help       - Show this help message"

