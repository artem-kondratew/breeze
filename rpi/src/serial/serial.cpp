#include "serial/serial.hpp"


Serial::Serial(std::string port, size_t baudrate, size_t cmd_data_size, size_t msg_data_size) {
    fd_ = open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

    tcgetattr(fd_, &serial_port_settings_);

    serial_port_settings_.c_cflag |= (CLOCAL | CREAD);    // Ignore modem controls
    serial_port_settings_.c_cflag &= ~CSIZE;
    serial_port_settings_.c_cflag |= CS8;     // 8 bit chars
    serial_port_settings_.c_cflag &= ~(PARENB | PARODD);  // shut off parody
    serial_port_settings_.c_cflag &= ~CSTOPB; //no scts stop
    serial_port_settings_.c_iflag &= ~IGNBRK; //disable break processing
    serial_port_settings_.c_iflag = 0;        // no echo
    serial_port_settings_.c_iflag &= ~(IXON | IXOFF | IXANY); // no software flow control
    serial_port_settings_.c_oflag = 0;        // no remapping
    serial_port_settings_.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG |IEXTEN);
    serial_port_settings_.c_cc[VMIN] = 0;     // read doesn't block
    serial_port_settings_.c_cc[VTIME] = 0;    // 0s read timeout

    cfsetspeed(&serial_port_settings_, baudrate);

    tcsetattr(fd_, TCSANOW, &serial_port_settings_);

    port_ = port;
    baudrate_ = baudrate;
    cmd_data_size_ = cmd_data_size;
    msg_data_size_ = msg_data_size;
    cmd_size_ = Msg::SERVICE_BYTES + cmd_data_size_;
    msg_size_ = Msg::SERVICE_BYTES + msg_data_size_;

    is_feedback_correct_ = false;
    if (fd_ != -1) {
        is_opened_ = true;
    }

    verbose_ = true;
}


Serial::~Serial() {
    disconnect();
}


uint8_t Serial::crc8(const uint8_t* pocket, size_t size) {
    uint8_t BYTE_SIZE = 8;
    uint8_t MSB_MASK = 0x80;
    uint8_t byte;
    uint8_t POLY = 0x7;
    uint8_t crc8 = 0xFF;

    for (size_t cell = 0; cell < size; cell++) {

        byte = pocket[cell];
        crc8 = crc8 ^ byte;

        for (int byte_number = 0; byte_number < BYTE_SIZE; byte_number++) {

            if (crc8 & MSB_MASK) {
                crc8 = (crc8 << 1) ^ POLY;
            }
            else {
                crc8 = crc8 << 1;
            }
        }
    }
    return crc8;
}


void Serial::setChecksumForSend(Msg* msg) {
    uint8_t checksum = crc8(msg->storage(), msg->size() - 1);
    msg->setChecksum(checksum);
}


bool Serial::checkChecksumFromReceive(uint8_t buffer[], size_t size) {
    return crc8(buffer, size) == 0;
}


void Serial::delay(size_t ms) {
    auto start_timer = std::chrono::system_clock::now();
    while (true) {
        auto end_timer = std::chrono::system_clock::now();
        auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(end_timer - start_timer).count();
        if (dt >= (int)ms) {
            return;
        }
    }
}


void Serial::send(Msg* msg) {
    if (msg->size() == 0) {
        if (verbose_) {
            std::cout << "EMPTY" << std::endl;
        }
        return;
    }
    
    msg->setStartBytes();
    setChecksumForSend(msg);

    if (verbose_) {
        std::cout << "cmd (size = " << msg->size() << ") ";
        msg->print();
    }
    
    write(fd_, msg->storage(), msg->size());
}


Msg Serial::receive(size_t data_size) {
    Msg msg(data_size);
    int res = read(fd_, msg.storage(), msg.size());

    is_feedback_correct_ = false;

    if (verbose_) {
        std::cout << "msg (size = " << msg.size() << ") ";
        msg.print();
    }

    if (*(msg.storage() + Msg::START_BYTE_0_IDX) == Msg::START_BYTE && *(msg.storage() + Msg::START_BYTE_1_IDX) == Msg::START_BYTE) {
        if (checkChecksumFromReceive(msg.storage(), msg.size())) {

            is_feedback_correct_ = true;
            return msg;
        }
        if (verbose_) {
            std::cout <<"CHECKSUM ERROR" << std::endl;
        }
    }

    static bool even = false;
    if (verbose_) {
        std::string nop = !even ? "READ ERROR" : " READ ERROR";
        even = !even;
        std::cout << nop << std::endl;
    }
    
    return Msg(0);
}


bool Serial::checkFeedback() {
    return is_feedback_correct_;
}


bool Serial::isOpened() {
    return (fd_ != -1 && is_opened_);
}


bool Serial::connect() {
    if (!isOpened()) {
        if (verbose_) {
            std::cout << "not opened" << std::endl;
        }
        return false;
    }

    Msg ping_cmd(cmd_data_size_);
    
    auto start_timer = std::chrono::system_clock::now();
    while (!is_feedback_correct_) {
        auto end_timer = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(end_timer - start_timer).count() > int(TIMER_)) {
            send(&ping_cmd);
            receive(msg_data_size_);
            start_timer = std::chrono::system_clock::now();
        }
    }

    if (verbose_) {
        std::cout << "connected" << std::endl;
    }
    
    delay(100);
    return true;
}


void Serial::disconnect() {
    close(fd_);
    is_opened_ = false;

    if (verbose_) {
        std::cout << "disconnected" << std::endl;
    }
}


void Serial::int64_to_uint8arr(int64_t number, uint8_t* output) {
    std::memcpy(output, &number, sizeof(int64_t));
}


int64_t Serial::uint8arr_to_int64(uint8_t* data) {
    int64_t number;
    std::memcpy(&number, data, sizeof(int64_t));
    return number;
}


void Serial::float_to_uint8arr(float number, uint8_t* data) {
    memcpy(data, &number, sizeof(float));
}


float Serial::uint8arr_to_float(uint8_t* data) {
    float number;
    memcpy(&number, data, sizeof(float));
    return number;
}


std::string Serial::port() {
    return port_;
}


size_t Serial::baudrate() {
    return baudrate_;
}


size_t Serial::cmd_size() {
    return cmd_size_;
}


size_t Serial::cmd_data_size() {
    return cmd_size_ - Msg::SERVICE_BYTES;
}


size_t Serial::msg_size() {
    return msg_size_;
}


size_t Serial::msg_data_size() {
    return msg_size_ - Msg::SERVICE_BYTES;
}


void Serial::enableVerbose() {
    verbose_ = true;
}


void Serial::disableVerbose() {
    verbose_ = false;
}
