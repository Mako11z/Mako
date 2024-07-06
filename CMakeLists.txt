cmake_minimum_required(VERSION 3.10)
project(Mako)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Include directories for header files
include_directories(${CMAKE_SOURCE_DIR})

# Add executable target for your main.cpp
add_executable(main ${SRC_DIR}/main.cpp ${SRC_DIR}/lexer.cpp ${SRC_DIR}/parser.cpp)
# Copy test.txt to build directory
configure_file(${CMAKE_SOURCE_DIR}/src/test.txt ${CMAKE_BINARY_DIR}/test.txt COPYONLY)
# Add a test for your lexer or parser
add_test(NAME LexerTest COMMAND main)