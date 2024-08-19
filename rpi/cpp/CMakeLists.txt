cmake_minimum_required(VERSION 3.18)
project(breeze)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories("${CMAKE_SOURCE_DIR}/include")

set(HEADER_FILES
include/serial/commander.hpp
include/serial/msg.hpp
include/serial/serial.hpp
)

set(SERIAL_SOURCE
src/serial/main.cpp
src/serial/commander.cpp
src/serial/msg.cpp
src/serial/serial.cpp
src/serial/socket.cpp
)

set(SERIAL_SOURCE
src/video/send_video.cpp
)

add_executable(breeze "${SERIAL_SOURCE}")
add_executable(sender "${VIDEO_SOURCE}")

target_link_libraries(sender ${OpenCV_LIBS})

install(TARGETS breeze DESTINATION bin)
install(TARGETS sender DESTINATION bin)
