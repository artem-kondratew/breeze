cmake_minimum_required(VERSION 3.18)
project(Breeze)

set(CMAKE_CXX_STANDARD 20)

include_directories("${CMAKE_SOURCE_DIR}/include")

set(LIBRARY ncurses)

set(HEADER_FILES
include/serial/commander.hpp
include/serial/msg.hpp
include/serial/serial.hpp
include/gui/parser.hpp
include/gui/window.hpp
)

set(SOURCE
src/main.cpp
src/gui/window.cpp
)

set(SERIAL_SOURCE
src/serial/commander.cpp
src/serial/msg.cpp
src/serial/serial.cpp
src/serial/commander_python_wrapper.cpp
)

add_executable(breeze "${SOURCE}" "${SERIAL_SOURCE}")

add_library(commander SHARED "${SERIAL_SOURCE}")

target_link_libraries(breeze "${LIBRARY}")
