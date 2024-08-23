#include "serial/ros_serial.hpp"


RosSerial::RosSerial(std::string node_name) : Node(node_name) {
    this->declare_parameter("port", "");
    this->declare_parameter("baudrate", 0);
    this->declare_parameter("cmd_size", 0);
    this->declare_parameter("msg_size", 0);
    this->declare_parameter("pub_topic", "");
    this->declare_parameter("sub_topic", "");
    this->declare_parameter("arduino_reset_topic", "");
    this->declare_parameter("use_receiver", false);
    this->declare_parameter("reading_timeout", 0);

    port_ = this->get_parameter("port").as_string();
    baudrate_ = this->get_parameter("baudrate").as_int();
    cmd_size_ = this->get_parameter("cmd_size").as_int();
    msg_size_ = this->get_parameter("msg_size").as_int();
    std::string pub_topic = this->get_parameter("pub_topic").as_string();
    std::string sub_topic = this->get_parameter("sub_topic").as_string();
    std::string arduino_reset_topic = this->get_parameter("arduino_reset_topic").as_string();
    use_receiver_ = this->get_parameter("use_receiver").as_bool();
    reading_timeout_ = this->get_parameter("reading_timeout").as_int();

    RCLCPP_INFO(this->get_logger(), "port: %s", port_.c_str());
    RCLCPP_INFO(this->get_logger(), "baudrate: %ld", baudrate_);
    RCLCPP_INFO(this->get_logger(), "cmd_size: %ld", cmd_size_);
    RCLCPP_INFO(this->get_logger(), "msg_size: %ld", msg_size_);
    RCLCPP_INFO(this->get_logger(), "pub_topic: %s", pub_topic.c_str());
    RCLCPP_INFO(this->get_logger(), "sub_topic: %s", sub_topic.c_str());
    RCLCPP_INFO(this->get_logger(), "arduino_reset_topic: %s", arduino_reset_topic.c_str());
    RCLCPP_INFO(this->get_logger(), "use_receiver: %s", use_receiver_ ? "true" : "false");
    RCLCPP_INFO(this->get_logger(), "reading_timeout: %ld", reading_timeout_);

    this->connect();

    using namespace std::chrono_literals;
    using std::placeholders::_1;
    
    publisher_ = this->create_publisher<robot_msgs::msg::UInt8Vector>(pub_topic, 10);
    subscription_ = this->create_subscription<robot_msgs::msg::UInt8Vector>(sub_topic, 10, std::bind(&RosSerial::subscriptionCallback, this, _1));

    arduino_reset_pub_ = this->create_publisher<std_msgs::msg::Bool>(arduino_reset_topic, 10);

    reading_ping_timer_ = this->create_wall_timer(std::chrono::milliseconds(1000), std::bind(&RosSerial::readingPingCallback, this));

    first_ros_msg_ = true;
    last_reading_time_ = this->get_clock()->now();

    reading_thread_ = std::thread(&RosSerial::readingThread, this);
    reading_thread_.detach();
}


void RosSerial::connect() {
    delete serial_;
    serial_ = new Serial(port_, baudrate_, cmd_size_, msg_size_);
    if (serial_->isOpened()) {
        RCLCPP_INFO(this->get_logger(), "Connected to serial device '%s' with baudrate %ld", serial_->port().c_str(), serial_->baudrate());
    }
    else {
        RCLCPP_FATAL(this->get_logger(), "Unable to connect to serial device '%s' with baudrate %ld", serial_->port().c_str(), serial_->baudrate());
        rclcpp::shutdown();
    }

    serial_->connect();
}


RosSerial::~RosSerial() {
    delete serial_;
}


robot_msgs::msg::UInt8Vector RosSerial::createRosMsg(Msg* serial_msg) {
    auto ros_msg = robot_msgs::msg::UInt8Vector();

    ros_msg.data.resize(serial_msg->data_size());
    std::copy(ros_msg.data.data(), ros_msg.data.data() + serial_msg->data_size(), serial_msg->data());

    return ros_msg;
}


void RosSerial::readingThread() {
    if (!use_receiver_) {
        return;
    }

    while (rclcpp::ok()) {
        if (!serial_->isReadyToRead()) {
            continue;
        }

        Msg msg = serial_->receive(serial_->msg_data_size());
        if (serial_->checkFeedback()) {
            auto ros_message = createRosMsg(&msg);
            last_reading_time_ = this->get_clock()->now();
        }
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
    first_ros_msg_ = true;
    Msg serial_msg = createSerialMsg(ros_msg);
    serial_->send(&serial_msg);
}


void RosSerial::readingPingCallback() {    
    if (!first_ros_msg_) {
        return;
    }

    size_t dt = static_cast<size_t>((this->get_clock()->now().seconds() - last_reading_time_.seconds()) * 1000);

    if (dt > reading_timeout_) {
        RCLCPP_INFO(this->get_logger(), "READING TIMEOUT ERROR");
        auto msg = std_msgs::msg::Bool();
        msg.data = true;
        arduino_reset_pub_->publish(msg);
    }
}
