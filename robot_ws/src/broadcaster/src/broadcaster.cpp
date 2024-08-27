#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>

#include <geometry_msgs/msg/transform_stamped.hpp>
#include <rclcpp/rclcpp.hpp>
#include <robot_msgs/msg/nav_data.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <std_msgs/msg/bool.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>


class Broadcaster : public rclcpp::Node {
private:
    rclcpp::Subscription<robot_msgs::msg::NavData>::SharedPtr input_sub_;

    rclcpp::Publisher<sensor_msgs::msg::NavSatFix>::SharedPtr gnss_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr active_pub_;

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::TimerBase::SharedPtr active_timer_;

    std::unique_ptr<tf2_ros::TransformBroadcaster> tf2_broadcaster_;

    std::string map_frame_, base_footprint_frame_;

    robot_msgs::msg::NavData data_;
    tf2::Quaternion q_;

    bool data_init_ = false;

public:
    Broadcaster();

private:
    void inputCallback(const robot_msgs::msg::NavData& msg);
    void transformCallback();
    void activeCallback();
};


Broadcaster::Broadcaster() : Node("broadcaster") {
    using namespace std::chrono_literals;
    using std::placeholders::_1;

    this->declare_parameter("input_topic", "");
    this->declare_parameter("active_topic", "");
    this->declare_parameter("gnss_topic", "");
    this->declare_parameter("map_frame", "");
    this->declare_parameter("base_footprint_frame", "");

    std::string input_topic = this->get_parameter("input_topic").as_string();
    std::string active_topic = this->get_parameter("active_topic").as_string();
    std::string gnss_topic = this->get_parameter("gnss_topic").as_string();
    map_frame_ = this->get_parameter("map_frame").as_string();
    base_footprint_frame_ = this->get_parameter("base_footprint_frame").as_string();

    RCLCPP_INFO(this->get_logger(), "input_topic: '%s'", input_topic.c_str());
    RCLCPP_INFO(this->get_logger(), "active_topic: '%s'", active_topic.c_str());
    RCLCPP_INFO(this->get_logger(), "gnss_topic: '%s'", gnss_topic.c_str());
    RCLCPP_INFO(this->get_logger(), "map_frame: '%s'", map_frame_.c_str());
    RCLCPP_INFO(this->get_logger(), "base_footprint_frame: '%s'", base_footprint_frame_.c_str());

    input_sub_ = this->create_subscription<robot_msgs::msg::NavData>(input_topic, 10, std::bind(&Broadcaster::inputCallback, this, _1));

    gnss_pub_ = this->create_publisher<sensor_msgs::msg::NavSatFix>(gnss_topic, 10);
    active_pub_ = this->create_publisher<std_msgs::msg::Bool>(active_topic, 10);

    timer_ = this->create_wall_timer(10ms, std::bind(&Broadcaster::transformCallback, this));
    active_timer_ = this->create_wall_timer(1s, std::bind(&Broadcaster::activeCallback, this));

    tf2_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
}


void Broadcaster::inputCallback(const robot_msgs::msg::NavData& msg) {
    RCLCPP_INFO(this->get_logger(), "input");
    data_ = msg;
    data_init_ = true;
    q_.setRPY(0, 0, -msg.yaw.data);
    auto gnss_msg = msg.gnss;
    gnss_msg.header.stamp = this->get_clock()->now();
    gnss_pub_->publish(gnss_msg);
}


void Broadcaster::transformCallback() {
        if (!data_init_) {
            return;
        }
        geometry_msgs::msg::TransformStamped transform;
        transform.header.stamp = this->get_clock()->now();
        transform.header.frame_id = map_frame_;
        transform.child_frame_id = base_footprint_frame_;
        transform.transform.translation.x = 0;
        transform.transform.translation.y = 0;
        transform.transform.translation.z = 0;
        transform.transform.rotation.x = q_.x();
        transform.transform.rotation.y = q_.y();
        transform.transform.rotation.z = q_.z();
        transform.transform.rotation.w = q_.w();
        tf2_broadcaster_->sendTransform(transform);
}


void Broadcaster::activeCallback() {
    auto msg = std_msgs::msg::Bool();
    msg.data = true;
    active_pub_->publish(msg);
}


int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Broadcaster>()); 
    rclcpp::shutdown();
    return 0;
}
