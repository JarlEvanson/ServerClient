cmake_minimum_required(VERSION 3.0.0)
project(Sockets VERSION 0.1.0)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../$<$<CONFIG:Debug>:>)

add_compile_definitions(ENDIANNESS=${CMAKE_CXX_BYTE_ORDER})

add_executable(
    Server 
    src/server/server.cpp 
    src/common/sockets.cpp
    src/common/packet.cpp
    src/common/timer.cpp
    src/common/serialize.cpp
    src/common/log.cpp
)

include_directories(Server include)

add_executable(
    Client
    src/client/client.cpp 
    src/common/sockets.cpp
    src/common/packet.cpp
    src/common/timer.cpp
    src/common/serialize.cpp
)

include_directories(Client include)