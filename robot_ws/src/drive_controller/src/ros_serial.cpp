#include "rclcpp/rclcpp.hpp"

#include "serial/ros_serial.hpp"


int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<RosSerial>("drive_controller_serial"));
    rclcpp::shutdown();
    
    return 0;
}
