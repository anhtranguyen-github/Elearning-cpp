cmake_minimum_required(VERSION 3.10)
project(EnglishLearningPlatform VERSION 1.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific settings
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    set(PLATFORM_LIBS ws2_32)
else()
    set(PLATFORM_LIBS pthread)
endif()

# Find Qt5 for client GUI
find_package(Qt5 COMPONENTS Core Widgets QUIET)

if(Qt5_FOUND)
    message(STATUS "Qt5 found - Client GUI will be built")
    set(Qt5_USE_QTMAIN_LIBRARY ON)
    set(CMAKE_AUTOMOC ON)
    set(BUILD_CLIENT_GUI ON)
else()
    message(WARNING "Qt5 not found - Client GUI will not be built")
    set(BUILD_CLIENT_GUI OFF)
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Common source files (used by both server and client)
set(COMMON_SOURCES
    src/utils/Logger.cpp
    src/utils/Parser.cpp
    src/protocol/Protocol.cpp
    src/protocol/Network.cpp
)

# Database source files
set(DATABASE_SOURCES
    src/db/Database.cpp
)

# Server source files
set(SERVER_SOURCES
    ${COMMON_SOURCES}
    ${DATABASE_SOURCES}
    src/server/Server.cpp
    src/server/ClientHandler.cpp
    src/server/main.cpp
)

# Client source files (non-GUI parts)
set(CLIENT_SOURCES
    ${COMMON_SOURCES}
    src/client/Client.cpp
)

# Build server executable
add_executable(server ${SERVER_SOURCES})
target_link_libraries(server ${PLATFORM_LIBS})

# Set output directory
set_target_properties(server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Build console client (no Qt required)
set(CONSOLE_CLIENT_SOURCES
    ${CLIENT_SOURCES}
    src/client/ConsoleClient.cpp
    src/client/console_main.cpp
)

add_executable(console_client ${CONSOLE_CLIENT_SOURCES})
target_link_libraries(console_client ${PLATFORM_LIBS})

set_target_properties(console_client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Build client executable (Qt GUI version)
if(BUILD_CLIENT_GUI)
    set(CLIENT_GUI_SOURCES
        ${CLIENT_SOURCES}
        src/client/UI.cpp
        src/client/main.cpp
    )
    
    add_executable(client ${CLIENT_GUI_SOURCES})
    target_link_libraries(client 
        Qt5::Core 
        Qt5::Widgets
        ${PLATFORM_LIBS}
    )
    
    set_target_properties(client PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    
    # On Windows, set it as a GUI application
    if(WIN32)
        set_target_properties(client PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()
endif()

# Installation rules
install(TARGETS server console_client
    RUNTIME DESTINATION bin
)

if(BUILD_CLIENT_GUI)
    install(TARGETS client
        RUNTIME DESTINATION bin
    )
endif()

install(DIRECTORY config/ DESTINATION config)
install(DIRECTORY logs/ DESTINATION logs OPTIONAL)

# Print build summary
message(STATUS "")
message(STATUS "=== Build Configuration ===")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++17 standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Server: YES")
message(STATUS "Console Client: YES")
message(STATUS "Client GUI: ${BUILD_CLIENT_GUI}")
message(STATUS "Platform libs: ${PLATFORM_LIBS}")
message(STATUS "===========================")
message(STATUS "")

