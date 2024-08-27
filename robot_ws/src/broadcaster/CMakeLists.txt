cmake_minimum_required(VERSION 3.8)
project(broadcaster)

find_package(ament_cmake REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rclcpp REQUIRED)
find_package(robot_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(tf2_ros REQUIRED)

add_executable(broadcaster
    src/broadcaster.cpp
)

ament_target_dependencies(broadcaster
    geometry_msgs
    rclcpp
    robot_msgs
    sensor_msgs
    std_msgs
    tf2
    tf2_ros
)

install(DIRECTORY
    include src launch config
    DESTINATION share/${PROJECT_NAME}
)

install(TARGETS
    broadcaster
    DESTINATION lib/${PROJECT_NAME}
)

ament_package()
