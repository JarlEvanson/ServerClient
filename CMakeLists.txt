cmake_minimum_required(VERSION 3.0.0)
project(Sockets VERSION 0.1.0)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_LIST_DIR}>)

add_compile_definitions(ENDIANNESS=${CMAKE_CXX_BYTE_ORDER})

add_executable(Server src/server/server.cpp src/common/sockets.cpp src/common/packet.cpp)

include_directories(Server include)

add_executable(Client src/client/client.cpp src/common/sockets.cpp src/common/packet.cpp)