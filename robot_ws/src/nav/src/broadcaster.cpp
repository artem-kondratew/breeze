#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>


#include <geometry_msgs/msg/transform_stamped.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <std_msgs/msg/float64.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>


class Broadcaster : public rclcpp::Node {
private:
    rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr gnss_sub_;
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr magnetometer_sub_;

    rclcpp::TimerBase::SharedPtr timer_;
    std::unique_ptr<tf2_ros::TransformBroadcaster> tf2_broadcaster_;

    std::string map_frame_, base_footprint_frame_;

    sensor_msgs::msg::NavSatFix gnss_data_;
    tf2::Quaternion magnetometer_data_;

    bool gnss_init_ = false;
    bool magnetometer_init_ = false;

public:
    Broadcaster();

private:
    void gnssCallback(const sensor_msgs::msg::NavSatFix& msg);
    void magnetometerCallback(const std_msgs::msg::Float64& msg);
    void transformCallback();
};


Broadcaster::Broadcaster() : Node("broadcaster") {
    using namespace std::chrono_literals;
    using std::placeholders::_1;

    this->declare_parameter("gnss_topic", "");
    this->declare_parameter("magnetometer_topic", "");
    this->declare_parameter("map_frame", "");
    this->declare_parameter("base_footprint_frame", "");

    std::string gnss_topic = this->get_parameter("gnss_topic").as_string();
    std::string magnetometer_topic = this->get_parameter("magnetometer_topic").as_string();
    map_frame_ = this->get_parameter("map_frame").as_string();
    base_footprint_frame_ = this->get_parameter("base_footprint_frame").as_string();

    RCLCPP_INFO(this->get_logger(), "gnss_topic: '%s'", gnss_topic.c_str());
    RCLCPP_INFO(this->get_logger(), "magnetometer_topic: '%s'", magnetometer_topic.c_str());
    RCLCPP_INFO(this->get_logger(), "map_frame: '%s'", map_frame_.c_str());
    RCLCPP_INFO(this->get_logger(), "base_footprint_frame: '%s'", base_footprint_frame_.c_str());

    gnss_sub_ = this->create_subscription<sensor_msgs::msg::NavSatFix>(gnss_topic, 10, std::bind(&Broadcaster::gnssCallback, this, _1));
    magnetometer_sub_ = this->create_subscription<std_msgs::msg::Float64>(magnetometer_topic, 10, std::bind(&Broadcaster::magnetometerCallback, this, _1));

    timer_ = this->create_wall_timer(10ms, std::bind(&Broadcaster::transformCallback, this));

    tf2_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
}


void Broadcaster::gnssCallback(const sensor_msgs::msg::NavSatFix& msg) {
    RCLCPP_INFO(this->get_logger(), "gnss");
    gnss_data_ = msg;
    gnss_init_ = true;
}


void Broadcaster::magnetometerCallback(const std_msgs::msg::Float64& msg) {
    RCLCPP_INFO(this->get_logger(), "magnetometer");
    magnetometer_data_.setRPY(0, 0, -msg.data);
    magnetometer_init_ = true;
}


void Broadcaster::transformCallback() {
        if (!magnetometer_init_ || !gnss_init_) {
            return;
        }
        geometry_msgs::msg::TransformStamped transform;
        transform.header.stamp = this->get_clock()->now();
        transform.header.frame_id = map_frame_;
        transform.child_frame_id = base_footprint_frame_;
        transform.transform.translation.x = 0;
        transform.transform.translation.y = 0;
        transform.transform.translation.z = 0;
        transform.transform.rotation.x = magnetometer_data_.x();
        transform.transform.rotation.y = magnetometer_data_.y();
        transform.transform.rotation.z = magnetometer_data_.z();
        transform.transform.rotation.w = magnetometer_data_.w();
        tf2_broadcaster_->sendTransform(transform);
}


int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Broadcaster>()); 
    rclcpp::shutdown();
    return 0;
}