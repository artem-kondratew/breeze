#ifndef SERIAL_ROS_SERIAL_HPP
#define SERIAL_ROS_SERIAL_HPP


#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>

#include "robot_msgs/msg/u_int8_vector.hpp"

#include "serial.hpp"


class RosSerial : public rclcpp::Node {
private:
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<robot_msgs::msg::UInt8Vector>::SharedPtr publisher_;
    rclcpp::Subscription<robot_msgs::msg::UInt8Vector>::SharedPtr subscription_;
    Serial* serial_;
    bool use_receiver_;

public:
    RosSerial(std::string node_name);
    ~RosSerial();

private:
    robot_msgs::msg::UInt8Vector createRosMsg(Msg* serial_msg);
    Msg createSerialMsg(robot_msgs::msg::UInt8Vector ros_msg);

    void timerCallback();
    void subscriptionCallback(const robot_msgs::msg::UInt8Vector& ros_msg);
};


#endif // SERIAL_ROS_SERIAL_HPP
