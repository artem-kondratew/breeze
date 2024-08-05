#ifndef COMMANDS_HPP
#define COMMANDS_HPP


#include "serial/serial.hpp"


class Commander {
private:
    Serial* serial_;
    size_t data_size_;

public:
    static const int8_t MIN_VEL = -100;
    static const int8_t MAX_VEL = +100;

private:
    static const uint8_t LED_ON = 1;
    static const uint8_t LED_OFF = 2;
    static const uint8_t SET_MIN_PPM = 3;
    static const uint8_t SET_REVERSE_PPM = 4;
    static const uint8_t SET_MAX_PPM = 5;
    static const uint8_t ALLOW_EMA = 6;
    static const uint8_t SET_VELOCITIES = 7;
    static const uint8_t INIT_MOTORS = 8;

public:
    Commander() = default;
    Commander(Serial* serial) : serial_{serial}, data_size_{serial_->cmd_data_size()} {}

    bool createSerial(std::string port, size_t baudrate, size_t cmd_data_size, size_t msg_data_size);
    bool connectSerial();
    void disconnectSerial();
    void deleteSerial();
    void enableSerialVerbose();
    void disableSerialVerbose();

    void led_on();
    void led_off();

    void set_min_ppm();
    void set_reverse_ppm();
    void set_max_ppm();
    void allow_ema();

    void set_velocities(int8_t vel0, int8_t vel1);

    void init_motors();
};


#endif // COMMANDS_HPP