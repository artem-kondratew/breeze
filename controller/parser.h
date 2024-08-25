#ifndef PARSER_H
#define PARSER_H

#include "config_serial.h"
#include "motor.h"
#include "serial.h"
#include "tools.h"


class Parser {
public:
    static void callback(uint8_t* cmd);
};


void Parser::callback(uint8_t* cmd) {
    uint8_t cmd_byte = cmd[0];
    
    switch (cmd_byte) {
        case NOP:{ // heartbeat

        }break;

        case LED_ON:{ // ВКЛ светодиод на D12
          tools::ledOn();
        }break;

        case LED_OFF:{ // ВЫКЛ светодиод на D12
          tools::ledOff();
        }break;
        
        case SET_VELOCITIES:{ // Установить значение скоростей на моторах
          if (!motor0->initialized_ || !motor1->initialized_) return;
          int8_t vel0_percent = cmd[1] * (cmd[1] % 2 ? -1 : 1);
          int8_t vel1_percent = cmd[2] * (cmd[2] % 2 ? -1 : 1);
          Motor::set_velocity_percents(vel0_percent, vel1_percent);
        }break;

        case INIT_MOTORS:{ // Инициализация моторов
          tools::ledOn();
          Motor::init();
          tools::ledOff();
        }break;

        case SET_COLOUR:{ // Установить основной цвет
          tools::set_aled_colour(cmd[1]);
        }break;

        case SET_SECTOR :{ // Запись сектора
          tools::set_sector(cmd[1]);
        }break;

        case SET_ANGLE:{ // Получение угла сектора, вычисление и запись сектора
          tools::set_sector_ang(cmd[1]);
        }break; 
    }
}

#endif // PARSER_H
