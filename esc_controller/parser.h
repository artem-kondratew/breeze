#ifndef PARSER_H
#define PARSER_H


#include "mode.h"
#include "motor.h"


#define CHANGE_SPEED_CMD 64 // @
#define STOP_CMD         42 // *

#define VERBOSE true


class Parser {
public:
    static void parse(uint8_t* cmd);
};


void Parser::parse(uint8_t* cmd) {
    if (cmd[0] == CHANGE_SPEED_CMD) {
        int8_t vel0_percent = cmd[1] * (cmd[1] % 2 ? -1 : 1);
        int8_t vel1_percent = cmd[2] * (cmd[2] % 2 ? -1 : 1);
        Motor::set_velocity_percents(vel0_percent, vel1_percent);
#if VERBOSE
        Serial.print("parser vels: ");
        Serial.print(vel0_percent);
        Serial.print(" ");
        Serial.println(vel1_percent);
#endif
        return;
    }

    if (cmd[0] == STOP_CMD) {
        Motor::set_velocity_percents(0, 0);
#if VERBOSE
        Serial.println("stop");
#endif
        return;
    }

    Mode::set_mode(cmd[0]);
}


#endif // PARSER_H
