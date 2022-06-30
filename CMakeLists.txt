cmake_minimum_required(VERSION 3.0.0)
project(Sockets VERSION 0.1.0)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/jarl/Documents/Projects/C++/Sockets)

add_executable(Sockets src/sockets.cpp)

include_directories(Sockets include)