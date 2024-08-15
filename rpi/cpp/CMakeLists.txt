cmake_minimum_required(VERSION 3.18)
project(Breeze)

set(CMAKE_CXX_STANDARD 20)

include_directories("${CMAKE_SOURCE_DIR}/include")

set(HEADER_FILES
include/serial/commander.hpp
include/serial/msg.hpp
include/serial/serial.hpp
)

set(SOURCE
src/main.cpp
)

set(SERIAL_SOURCE
src/serial/commander.cpp
src/serial/msg.cpp
src/serial/serial.cpp
src/serial/socket.cpp
)

add_executable(breeze "${SOURCE}" "${SERIAL_SOURCE}")
