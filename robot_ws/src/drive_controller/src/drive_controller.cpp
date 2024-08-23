#include <iostream>
#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2/LinearMath/Quaternion.h>


#include <geometry_msgs/msg/twist.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <std_msgs/msg/float32.hpp>
#include <std_msgs/msg/int64.hpp>
#include <std_msgs/msg/bool.hpp>
#include "robot_msgs/msg/u_int8_vector.hpp"
#include "robot_msgs/msg/float32_vector.hpp"


using namespace std::chrono_literals;
using std::placeholders::_1;


class DriveController : public rclcpp::Node {
private:
    static const int8_t MIN_VEL = -100;
    static const int8_t MAX_VEL = +100;

    static const size_t CMD_IDX = 0;
    static const size_t V0_SIGN_IDX = 1;
    static const size_t V0_IDX = 2;
    static const size_t V1_SIGN_IDX = 3;
    static const size_t V1_IDX = 4;

    static const size_t SERIAL_CMD_IDX = 0;
    static const size_t SERIAL_V0_IDX = 1;
    static const size_t SERIAL_V1_IDX = 2;

    static const uint8_t LED_ON = 1;
    static const uint8_t LED_OFF = 2;
    static const uint8_t SET_VELOCITIES = 3;
    static const uint8_t INIT_MOTORS = 4;


    rclcpp::Publisher<robot_msgs::msg::UInt8Vector>::SharedPtr serial_pub_;
    // rclcpp::Subscription<robot_msgs::msg::UInt8Vector>::SharedPtr serial_sub_;
    rclcpp::Subscription<robot_msgs::msg::UInt8Vector>::SharedPtr input_sub_;

    size_t cmd_size_, pose_num_, pose_size_, vel_num_, vel_size_;
    robot_msgs::msg::UInt8Vector serial_msg;


public:
    DriveController();

private:
    void led_on();
    void led_off();
    void set_velocities(int8_t vel0, int8_t vel1);
    void init_motors();

    void cmdCallback(const robot_msgs::msg::UInt8Vector& msg);
};


DriveController::DriveController() : Node("drive_controller") {
    this->declare_parameter("input_sub", "");
    this->declare_parameter("serial_pub", "");
    this->declare_parameter("serial_sub", "");
    this->declare_parameter("cmd_size", 0);

    std::string input_sub_topic = this->get_parameter("input_sub").as_string();
    std::string serial_pub_topic = this->get_parameter("serial_pub").as_string();
    std::string serial_sub_topic = this->get_parameter("serial_sub").as_string();
    cmd_size_ = this->get_parameter("cmd_size").as_int();

    RCLCPP_INFO(this->get_logger(), "input_sub_topic: '%s'", input_sub_topic.c_str());
    RCLCPP_INFO(this->get_logger(), "serial_pub_topic: '%s'", serial_pub_topic.c_str());
    RCLCPP_INFO(this->get_logger(), "serial_sub_topic: '%s'", serial_sub_topic.c_str());
    RCLCPP_INFO(this->get_logger(), "cmd_size: %ld", cmd_size_);

    serial_pub_ = this->create_publisher<robot_msgs::msg::UInt8Vector>(serial_sub_topic, 10);

    // serial_sub_ = this->create_subscription<robot_msgs::msg::UInt8Vector>(serial_pub_topic, 10, std::bind(&DriveController::odomCallback, this, _1));
    input_sub_ = this->create_subscription<robot_msgs::msg::UInt8Vector>(input_sub_topic, 10, std::bind(&DriveController::cmdCallback, this, _1));

    serial_msg.data.resize(cmd_size_);
}


void DriveController::led_on() {
    serial_msg.data[SERIAL_CMD_IDX] = LED_ON;
    serial_msg.data[SERIAL_V0_IDX] = 0;
    serial_msg.data[SERIAL_V1_IDX] = 0;
    serial_pub_->publish(serial_msg);
}


void DriveController::led_off() {
    serial_msg.data[SERIAL_CMD_IDX] = LED_OFF;
    serial_msg.data[SERIAL_V0_IDX] = 0;
    serial_msg.data[SERIAL_V1_IDX] = 0;
    serial_pub_->publish(serial_msg);
}


template <typename T>
T constrain(T data, T min, T max) {
    data = data > max ? max : data;
    data = data < min ? min : data;
    return data;
}


void DriveController::set_velocities(int8_t vel0, int8_t vel1) {
    vel0 = constrain(vel0, MIN_VEL, MAX_VEL);
    vel1 = constrain(vel1, MIN_VEL, MAX_VEL);

    uint8_t v0, v1;

    if (vel0 < 0) {
        v0 = std::abs(vel0) % 2 ? std::abs(vel0) : std::abs(vel0) + 1;
    }
    else {
        v0 = vel0 % 2 ? vel0 + 1 : vel0;
    }

    if (vel1 < 0) {
        v1 = std::abs(vel1) % 2 ? std::abs(vel1) : std::abs(vel1) + 1;
    }
    else {
        v1 = vel1 % 2 ? vel1 + 1 : vel1;
    }

    serial_msg.data[SERIAL_CMD_IDX] = SET_VELOCITIES;
    serial_msg.data[SERIAL_V0_IDX] = v0;
    serial_msg.data[SERIAL_V1_IDX] = v1;

    serial_pub_->publish(serial_msg);
}


void DriveController::init_motors() {
    serial_msg.data[SERIAL_CMD_IDX] = INIT_MOTORS;
    serial_msg.data[SERIAL_V0_IDX] = 0;
    serial_msg.data[SERIAL_V1_IDX] = 0;
    serial_pub_->publish(serial_msg);
}


void DriveController::cmdCallback(const robot_msgs::msg::UInt8Vector& msg) {
    std::cout << (int)msg.data[0] << (int)msg.data[1] << (int)msg.data[2] << std::endl;
    uint8_t command = msg.data[CMD_IDX];
    int8_t v0 = msg.data[V0_SIGN_IDX] ? msg.data[V0_IDX] : -msg.data[V0_IDX];
    int8_t v1 = msg.data[V1_SIGN_IDX] ? msg.data[V1_IDX] : -msg.data[V1_IDX];

    if (command == SET_VELOCITIES) {
        return set_velocities(v0, v1);
    }
    if (command == LED_ON) {
        return led_on();
    }
    if (command == LED_OFF) {
        return led_off();
    }
    if (command == INIT_MOTORS) {
        return init_motors();
    }
}


int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DriveController>()); 
    rclcpp::shutdown();

    return 0;
}
