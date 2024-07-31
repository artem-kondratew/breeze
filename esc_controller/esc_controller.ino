#include "parser.h"
#include "motor.h"


#define MOTOR_0_DATA_PIN 10
#define MOTOR_1_DATA_PIN 11

#define ESC_0_POWER_PIN  A0
#define ESC_1_POWER_PIN  A1

#define MIN_PPM          1000
#define MAX_PPM          1976
#define REVERSE_PPM      1500

#define MIN_VEL          17
#define MIN_VEL_REV      -43

#define CMD_SIZE         3


uint8_t cmd[3];


void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(ESC_0_POWER_PIN, INPUT);
    pinMode(ESC_1_POWER_PIN, INPUT);
    
    digitalWrite(LED_BUILTIN, LOW);
    
    Serial.begin(9600);
    Serial.setTimeout(50);

    motor0 = new Motor(MIN_PPM, MAX_PPM, REVERSE_PPM, MIN_VEL, MIN_VEL_REV, MOTOR_0_DATA_PIN, ESC_0_POWER_PIN);
    motor1 = new Motor(MIN_PPM, MAX_PPM, REVERSE_PPM, MIN_VEL, MIN_VEL_REV, MOTOR_1_DATA_PIN, ESC_1_POWER_PIN);

    Motor::init();
    Mode::init();
}


void loop() {
    if (Serial.available() >= CMD_SIZE) {
        Serial.print("cmd: ");
        for (int i = 0; i < CMD_SIZE; i++) {
            cmd[i] = Serial.read();
            Serial.print(int(cmd[i]));
            Serial.print(" ");
        }
        Serial.println("");
        
        // motor0.set_ppm(vel);
        // motor1.set_ppm(vel);
        
        // motor0.set_velocity(vel);
        // motor1.set_velocity(vel);
        
        // motor0->set_velocity_percents(vel);
        // motor1->set_velocity_percents(vel);

        Parser::parse(cmd);

        Serial.println("");
    }
}
