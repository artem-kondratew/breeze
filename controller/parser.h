#ifndef PARSER_H
#define PARSER_H


#include "mode.h"
#include "motor.h"
#include "serial.h"
#include "tools.h"


#define NOP             0
#define LED_ON          1
#define LED_OFF         2
#define SET_MIN_PPM     3
#define SET_REVERSE_PPM 4
#define SET_MAX_PPM     5
#define ALLOW_EMA       6
#define SET_VELOCITIES  7
#define INIT_MOTORS     8


class Parser {
public:
    static void callback(uint8_t* cmd);
};


void Parser::callback(uint8_t* cmd) {
    uint8_t cmd_byte = cmd[0];
    
    if (cmd_byte == LED_ON) {
        tools::ledOn();
    }
    if (cmd_byte == LED_OFF) {
        tools::ledOff();
    }
    
    if (cmd_byte == SET_MIN_PPM) {
        Motor::disableEMAs();
        Motor::set_min_ppms();
    }
    if (cmd_byte == SET_REVERSE_PPM) {
        Motor::disableEMAs();
        Motor::set_reverse_ppms();
    }
    if (cmd_byte == SET_MAX_PPM) {
        Motor::disableEMAs();
        Motor::set_max_ppms();
    }
    if (cmd_byte == ALLOW_EMA) {
        Motor::enableEMAs();
    }
    
    if (cmd_byte == SET_VELOCITIES) {
        if (!motor0->initialized_ || !motor1->initialized_) {
            return;
        }
        int8_t vel0_percent = cmd[1] * (cmd[1] % 2 ? -1 : 1);
        int8_t vel1_percent = cmd[2] * (cmd[2] % 2 ? -1 : 1);
        return Motor::set_velocity_percents(vel0_percent, vel1_percent);
    }

    if (cmd_byte == INIT_MOTORS) {
        tools::ledOn();
        Motor::init();
        Mode::init();
        tools::ledOff();
    }

//    Mode::set_mode(cmd[0]);
}


#endif // PARSER_H
