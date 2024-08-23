#include "serial/ros_serial.hpp"


RosSerial::RosSerial(std::string node_name) : Node(node_name) {
    this->declare_parameter("port", "");
    this->declare_parameter("baudrate", 0);
    this->declare_parameter("cmd_size", 0);
    this->declare_parameter("msg_size", 0);
    this->declare_parameter("receive_time", 0);
    this->declare_parameter("pub_topic", "");
    this->declare_parameter("sub_topic", "");
    this->declare_parameter("use_receiver", false);

    std::string port = this->get_parameter("port").as_string();
    size_t baudrate = this->get_parameter("baudrate").as_int();
    size_t cmd_size = this->get_parameter("cmd_size").as_int();
    size_t msg_size = this->get_parameter("msg_size").as_int();
    size_t receive_time = this->get_parameter("receive_time").as_int();
    std::string pub_topic = this->get_parameter("pub_topic").as_string();
    std::string sub_topic = this->get_parameter("sub_topic").as_string();
    use_receiver_ = this->get_parameter("use_receiver").as_bool();

    RCLCPP_INFO(this->get_logger(), "port: %s", port.c_str());
    RCLCPP_INFO(this->get_logger(), "baudrate: %ld", baudrate);
    RCLCPP_INFO(this->get_logger(), "cmd_size: %ld", cmd_size);
    RCLCPP_INFO(this->get_logger(), "msg_size: %ld", msg_size);
    RCLCPP_INFO(this->get_logger(), "receive_time: %ld", receive_time);
    RCLCPP_INFO(this->get_logger(), "pub_topic: %s", pub_topic.c_str());
    RCLCPP_INFO(this->get_logger(), "sub_topic: %s", sub_topic.c_str());
    RCLCPP_INFO(this->get_logger(), "sub_topic: %s", use_receiver_ ? "true" : "false");

    serial_ = new Serial(port, baudrate, cmd_size, msg_size);
    if (serial_->isOpened()) {
        RCLCPP_INFO(this->get_logger(), "Connected to serial device '%s' with baudrate %ld", serial_->port().c_str(), serial_->baudrate());
    }
    else {
        RCLCPP_FATAL(this->get_logger(), "Unable to connect to serial device '%s' with baudrate %ld", serial_->port().c_str(), serial_->baudrate());
        rclcpp::shutdown();
    }

    serial_->connect();

    using namespace std::chrono_literals;
    using std::placeholders::_1;
    publisher_ = this->create_publisher<robot_msgs::msg::UInt8Vector>(pub_topic, 10);
    timer_ = this->create_wall_timer(std::chrono::milliseconds(receive_time), std::bind(&RosSerial::timerCallback, this));
    subscription_ = this->create_subscription<robot_msgs::msg::UInt8Vector>(sub_topic, 10, std::bind(&RosSerial::subscriptionCallback, this, _1));
}


RosSerial::~RosSerial() {
    delete serial_;
}


robot_msgs::msg::UInt8Vector RosSerial::createRosMsg(Msg* serial_msg) {
    auto ros_msg = robot_msgs::msg::UInt8Vector();

    for (size_t i = 0; i < serial_msg->data_size() - 1; i++) {
        uint8_t data = serial_msg->operator[](i);
        ros_msg.data.push_back(data);
    }

    return ros_msg;
}


void RosSerial::timerCallback() {
    if (!use_receiver_) {
        return;
    }

    Msg msg = serial_->receive(serial_->msg_size());
    if (serial_->checkFeedback()) {
        auto ros_message = createRosMsg(&msg);
        publisher_->publish(ros_message);
    }
}


Msg RosSerial::createSerialMsg(robot_msgs::msg::UInt8Vector ros_msg) {
    Msg serial_msg(ros_msg.data.size());

    for (size_t i = 0; i < ros_msg.data.size(); i++) {
        serial_msg[i] = ros_msg.data[i];
    }
    
    return serial_msg;
}


void RosSerial::subscriptionCallback(const robot_msgs::msg::UInt8Vector& ros_msg) {
    Msg serial_msg = createSerialMsg(ros_msg);
    serial_->send(&serial_msg);
}
