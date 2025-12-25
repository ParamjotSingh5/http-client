# 1. Set the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# 2. Name your project
project(AddressExplorer)

# 3. Specify the C++ standard (C++17 is usually a safe, modern choice)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. Create the executable
# Syntax: add_executable(ExecutableName source_file.cpp)
file(GLOB SRC_FILES "./src/*.cpp")
add_executable(tcpClient ${SRC_FILES})