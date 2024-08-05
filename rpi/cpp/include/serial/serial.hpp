#ifndef SERIAL_SERIAL_HPP
#define SERIAL_SERIAL_HPP


#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <iostream>
#include <string>

#include "msg.hpp"


inline uint8_t i = 0;

class Serial {
private:
    static const size_t TIMER_ = 1000;

    struct termios serial_port_settings_;

    std::string port_;
    size_t baudrate_;
    size_t cmd_size_;
    size_t msg_size_;
    size_t cmd_data_size_;
    size_t msg_data_size_;
    int fd_;

    bool is_opened_;
    bool is_feedback_correct_;
    bool verbose_;

public:
    Serial(std::string port, size_t baudrate, size_t cmd_data_size, size_t msg_data_size);
    ~Serial();
    Serial(const Serial& other) = delete;
    Serial(Serial&& other) = delete;

    Serial* operator=(const Serial& other) = delete;
    Serial* operator=( Serial&& other) = delete;

private:
    static uint8_t crc8(const uint8_t* data, uint64_t size);
    static void setChecksumForSend(Msg* msg);
    static bool checkChecksumFromReceive(uint8_t buffer[], size_t size);

public:
    static void delay(size_t ms);

    void send(Msg* msg);
    Msg receive(size_t size);

    bool checkFeedback();
    bool isOpened();
    bool connect();
    void disconnect();

    static void int64_to_uint8arr(int64_t number, uint8_t* output);
    static int64_t uint8arr_to_int64(uint8_t* data);
    static void float_to_uint8arr(float number, uint8_t* data);
    static float uint8arr_to_float(uint8_t* data);

    std::string port();
    size_t baudrate();
    size_t cmd_size();
    size_t cmd_data_size();
    size_t msg_size();
    size_t msg_data_size();

    void enableVerbose();
    void disableVerbose();
};


#endif // SERIAL_SERIAL_HPP
