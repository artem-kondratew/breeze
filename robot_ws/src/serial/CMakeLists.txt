cmake_minimum_required(VERSION 3.8)
project(serial)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(robot_msgs REQUIRED)

include_directories(include)

add_library(${PROJECT_NAME}
    include/serial/serial.hpp
    src/serial.cpp
    include/serial/msg.hpp
    src/msg.cpp
    include/serial/ros_serial.hpp
    src/ros_serial.cpp
)

ament_target_dependencies(${PROJECT_NAME} rclcpp robot_msgs)

ament_export_targets(${PROJECT_NAME} HAS_LIBRARY_TARGET)

install(DIRECTORY
    src include/serial
    DESTINATION include
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

ament_package()
