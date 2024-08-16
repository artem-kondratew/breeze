cmake_minimum_required(VERSION 3.18)
project(breeze)

set(CMAKE_CXX_STANDARD 20)

include_directories("${CMAKE_SOURCE_DIR}/include")

set(HEADER_FILES
include/serial/commander.hpp
include/serial/msg.hpp
include/serial/serial.hpp
)

set(SOURCE
src/main.cpp
src/serial/commander.cpp
src/serial/msg.cpp
src/serial/serial.cpp
src/serial/socket.cpp
)

add_executable(breeze "${SOURCE}")

install(TARGETS breeze DESTINATION bin)
