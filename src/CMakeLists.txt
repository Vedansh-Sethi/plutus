cmake_minimum_required(VERSION 3.15)
project(plutus CXX)

file(GLOB UTILS CONFIGURE_DEPENDS "utils/*.cpp")

set(SOURCES
    main.cpp
    ${UTILS}
)

# Define the executable
add_executable(plutus ${SOURCES})

# Include the current directory for header lookups
target_include_directories(plutus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})