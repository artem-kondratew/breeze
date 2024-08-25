#ifndef PARSER_H
#define PARSER_H


#include "mode.h"
#include "motor.h"
#include "serial.h"
#include "tools.h"


#define NOP             0
#define LED_ON          1
#define LED_OFF         2
#define SET_VELOCITIES  3
#define INIT_MOTORS     4


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
}


#endif // PARSER_H
