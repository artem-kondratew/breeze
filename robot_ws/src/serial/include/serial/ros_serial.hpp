#ifndef SERIAL_ROS_SERIAL_HPP
#define SERIAL_ROS_SERIAL_HPP


#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/bool.hpp>
#include "robot_msgs/msg/u_int8_vector.hpp"

#include "serial.hpp"


class RosSerial : public rclcpp::Node {
private:
    rclcpp::Publisher<robot_msgs::msg::UInt8Vector>::SharedPtr publisher_;
    rclcpp::Subscription<robot_msgs::msg::UInt8Vector>::SharedPtr subscription_;

    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr arduino_reset_pub_;

    rclcpp::TimerBase::SharedPtr reading_ping_timer_;

    std::string port_;
    size_t baudrate_;
    size_t cmd_size_;
    size_t msg_size_;

    Serial* serial_;
    bool use_receiver_;

    std::thread reading_thread_;

    rclcpp::Time last_reading_time_;
    bool first_ros_msg_;
    size_t reading_timeout_;

public:
    RosSerial(std::string node_name);
    ~RosSerial();

private:
    void connect();

    robot_msgs::msg::UInt8Vector createRosMsg(Msg* serial_msg);
    Msg createSerialMsg(robot_msgs::msg::UInt8Vector ros_msg);

    void readingThread();
    void subscriptionCallback(const robot_msgs::msg::UInt8Vector& ros_msg);
    void readingPingCallback();
};


#endif // SERIAL_ROS_SERIAL_HPP
