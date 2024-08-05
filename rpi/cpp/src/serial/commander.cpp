#include "serial/commander.hpp"


bool Commander::createSerial(std::string port, size_t baudrate, size_t cmd_data_size, size_t msg_data_size) {
    serial_ = new Serial(port, baudrate, cmd_data_size, msg_data_size);
    data_size_ = cmd_data_size;
    return serial_->isOpened();
}


bool Commander::connectSerial() {
    if (!serial_) {
        return false;
    }
    return serial_->connect();
}


void Commander::disconnectSerial() {
    serial_->disconnect();
}


void Commander::deleteSerial() {
    delete serial_;
}


void Commander::enableSerialVerbose() {
    serial_->disableVerbose();
}


void Commander::disableSerialVerbose() {
    serial_->disableVerbose();
}


void Commander::led_on() {
    Msg msg(data_size_, {LED_ON, 0, 0});
    serial_->send(&msg);
}


void Commander::led_off() {
    Msg msg(data_size_, {LED_OFF, 0, 0});
    serial_->send(&msg);
}


void Commander::set_min_ppm() {
    Msg msg(data_size_, {SET_MIN_PPM, 0, 0});
    serial_->send(&msg);
}


void Commander::set_reverse_ppm() {
    Msg msg(data_size_, {SET_REVERSE_PPM, 0, 0});
    serial_->send(&msg);
}


void Commander::set_max_ppm() {
    Msg msg(data_size_, {SET_MAX_PPM, 0, 0});
    serial_->send(&msg);
}


void Commander::allow_ema() {
    Msg msg(data_size_, {ALLOW_EMA, 0, 0});
    serial_->send(&msg);
}


template <typename T>
T constrain(T data, T min, T max) {
    data = data > max ? max : data;
    data = data < min ? min : data;
    return data;
}


void Commander::set_velocities(int8_t vel0, int8_t vel1) {
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

    Msg msg(data_size_, {SET_VELOCITIES, v0, v1});
    serial_->send(&msg);
}


void Commander::init_motors() {
    Msg msg(data_size_, {INIT_MOTORS, 0, 0});
    serial_->send(&msg);
}
