cmake_minimum_required(VERSION 3.18)
project(breeze)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories("${CMAKE_SOURCE_DIR}/include")

set(HEADER_FILES

)

set(SOURCE
src/video/receive_video.cpp
)

add_executable(receiver "${SOURCE}")

target_link_libraries(receiver ${OpenCV_LIBS})

install(TARGETS receiver DESTINATION bin)
