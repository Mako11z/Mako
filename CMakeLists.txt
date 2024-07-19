cmake_minimum_required(VERSION 3.10)
project(Mako)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(LLVM_DIR "/opt/homebrew/opt/llvm/lib/cmake/llvm")

# Find LLVM package
find_package(LLVM REQUIRED CONFIG)

# Set LLVM include directories
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})


# Include directories for header files
include_directories(${CMAKE_SOURCE_DIR})
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Add executable target for your main.cpp
add_executable(main ${SRC_DIR}/main.cpp ${SRC_DIR}/lexer.cpp ${SRC_DIR}/parser.cpp ${SRC_DIR}/codegen.cpp)
# Link against LLVM libraries
llvm_map_components_to_libnames(LLVM_LIBS core support irreader)
target_link_libraries(main ${LLVM_LIBS})
# Copy test.txt to build directory
configure_file(${CMAKE_SOURCE_DIR}/src/test.txt ${CMAKE_BINARY_DIR}/test.txt COPYONLY)
# Add a test for your lexer or parser
add_test(NAME LexerTest COMMAND main)