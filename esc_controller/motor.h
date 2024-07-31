#ifndef MOTOR_H
#define MOTOR_H


#include <Servo.h>

#include "mode.h"


#define VERBOSE true


class Motor {
private:
    Servo esc_;
    
    uint16_t min_ppm_;
    uint16_t max_ppm_;
    uint16_t reverse_ppm_;
    
    int16_t min_vel_;
    int16_t min_vel_rev_;
    int16_t max_vel_;
    int16_t max_vel_rev_;

    uint8_t data_pin_;
    uint8_t esc_power_pin_;
    
public:
    Motor(uint16_t min_ppm, uint16_t max_ppm, uint16_t reverse_ppm, uint16_t min_vel, uint16_t min_vel_rev, uint8_t data_pin, uint8_t esc_power_pin);

    static void init();
    
    void set_ppm(int16_t ppm);
    void set_velocity(int16_t vel);
    void set_velocity_percents(int8_t percent);

    static void set_velocity_percents(int8_t percent0, int8_t percent1);
    static void set_bounds(int8_t* bounds);
};


Motor* motor0;
Motor* motor1;
int8_t* percent_bounds_;


void blink(size_t cnt, size_t dt) {
    for (size_t i = 0; i < cnt; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(dt);
        digitalWrite(LED_BUILTIN, LOW);
        delay(dt);
    }
}


Motor::Motor(uint16_t min_ppm, uint16_t max_ppm, uint16_t reverse_ppm,
             uint16_t min_vel, uint16_t min_vel_rev, uint8_t data_pin, uint8_t esc_power_pin) : min_ppm_{min_ppm},
                                                                                                max_ppm_{max_ppm},
                                                                                                reverse_ppm_{reverse_ppm},
                                                                                                min_vel_{min_vel},
                                                                                                min_vel_rev_{min_vel_rev},
                                                                                                data_pin_{data_pin},
                                                                                                esc_power_pin_{esc_power_pin} {
    max_vel_ = max_ppm_ - reverse_ppm_ - min_vel;
    max_vel_rev_ = min_ppm_ - reverse_ppm_ - min_vel_rev_;
}


void Motor::init() {
    
    motor0->esc_.attach(motor0->data_pin_, motor0->min_ppm_, motor0->max_ppm_);
    motor1->esc_.attach(motor1->data_pin_, motor1->min_ppm_, motor1->max_ppm_);
    
    motor0->esc_.writeMicroseconds(motor0->max_ppm_);
    motor1->esc_.writeMicroseconds(motor1->max_ppm_);

    blink(3, 200);

    // while (analogRead(esc_power_pin) < 500) {}

    digitalWrite(LED_BUILTIN, HIGH);
    
    motor0->esc_.writeMicroseconds(motor0->reverse_ppm_);
    motor1->esc_.writeMicroseconds(motor1->reverse_ppm_);
    
    delay(2400);
    digitalWrite(LED_BUILTIN, LOW);
}


void Motor::set_ppm(int16_t ppm) {
    if (ppm < min_ppm_ || ppm > max_ppm_) {
        return;
    }
    
#if VERBOSE
    Serial.print("ppm: ");
    Serial.println(ppm);
#endif

    esc_.writeMicroseconds(ppm);
}


void Motor::set_velocity(int16_t vel) {
    uint16_t ppm;

    vel = constrain(vel, max_vel_rev_, max_vel_);

    if (vel == 0) {
        ppm = reverse_ppm_;
    }
    else {
        ppm = reverse_ppm_ + (vel > 0 ? min_vel_ : min_vel_rev_) + vel;
    }

#if VERBOSE
    Serial.print("ppm: ");
    Serial.println(ppm);
#endif

    esc_.writeMicroseconds(ppm);
}


void Motor::set_velocity_percents(int8_t percent) {
    percent = percent < 0 ? constrain(percent, percent_bounds_[0], percent_bounds_[1]) : constrain(percent, percent_bounds_[2], percent_bounds_[3]);
    int16_t vel = max_vel_ * (percent / 100.0);
    set_velocity(vel);

#if VERBOSE
    Serial.print("percent: ");
    Serial.println(percent);
#endif
}


void Motor::set_velocity_percents(int8_t percent0, int8_t percent1) {
    motor0->set_velocity_percents(percent0);
    motor1->set_velocity_percents(percent1);
}


void Motor::set_bounds(int8_t* bounds) {
    percent_bounds_ = bounds;
}


#endif // MOTOR_H
