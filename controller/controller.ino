#include "mode.h"
#include "motor.h"
#include "parser.h"
#include "serial.h"


#define CMD_SIZE                        3
#define MSG_SIZE                        1

#define MOTOR_0_DATA_PIN               10
#define MOTOR_1_DATA_PIN               11

#define ESC_0_POWER_PIN                 5
#define ESC_1_POWER_PIN                 6

#define MOTOR_0_MIN_PPM              1000
#define MOTOR_0_REVERSE_PPM          1500
#define MOTOR_0_MAX_PPM              1956

#define MOTOR_1_MIN_PPM              1000
#define MOTOR_1_REVERSE_PPM          1500
#define MOTOR_1_MAX_PPM              1956

#define MOTOR_0_REAL_REVERSE_PPM_NEG 1457
#define MOTOR_0_REAL_REVERSE_PPM_POS 1517

#define MOTOR_1_REAL_REVERSE_PPM_NEG 1457
#define MOTOR_1_REAL_REVERSE_PPM_POS 1518

#define MOTOR_0_MIN_VEL MOTOR_0_REAL_REVERSE_PPM_POS - MOTOR_0_REVERSE_PPM
#define MOTOR_0_MIN_VEL_REV MOTOR_0_REAL_REVERSE_PPM_NEG - MOTOR_0_REVERSE_PPM

#define MOTOR_1_MIN_VEL MOTOR_1_REAL_REVERSE_PPM_POS - MOTOR_1_REVERSE_PPM
#define MOTOR_1_MIN_VEL_REV MOTOR_1_REAL_REVERSE_PPM_NEG - MOTOR_1_REVERSE_PPM


uint64_t start_time;


void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(MOTOR_0_DATA_PIN, OUTPUT);
    pinMode(MOTOR_1_DATA_PIN, OUTPUT);
    pinMode(ESC_0_POWER_PIN, INPUT);
    pinMode(ESC_1_POWER_PIN, INPUT);
    
    digitalWrite(LED_BUILTIN, LOW);
    
    serial::init(CMD_SIZE, MSG_SIZE, Parser::callback);
    serial::connect();
    
    motor0 = new Motor(0, MOTOR_0_MIN_PPM, MOTOR_0_MAX_PPM, MOTOR_0_REVERSE_PPM, MOTOR_0_MIN_VEL, MOTOR_0_MIN_VEL_REV, MOTOR_0_DATA_PIN, ESC_0_POWER_PIN);
    motor1 = new Motor(1, MOTOR_1_MIN_PPM, MOTOR_1_MAX_PPM, MOTOR_1_REVERSE_PPM, MOTOR_1_MIN_VEL, MOTOR_1_MIN_VEL_REV, MOTOR_1_DATA_PIN, ESC_1_POWER_PIN);
    
    start_time = millis();
}


void loop() {
    Motor::spin();
    serial::receive();

    if (millis() - start_time >= 100) {
        serial::ok();
        start_time = millis();
    }
}
