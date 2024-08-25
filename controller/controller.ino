
#include "motor.h"
#include "parser.h"
#include "serial.h"


void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(MOTOR_0_DATA_PIN, OUTPUT);
    pinMode(MOTOR_1_DATA_PIN, OUTPUT);
    
    digitalWrite(LED_BUILTIN, LOW);

    tools::init_aled();
    
    serial::init(CMD_SIZE, MSG_SIZE, Parser::callback);
    serial::connect();
}


void loop() {
    Motor::spin();
    serial::receive();
    tools::update_aled();
}
