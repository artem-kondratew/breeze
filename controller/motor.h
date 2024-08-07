#ifndef MOTOR_H
#define MOTOR_H


#include <Servo.h>

#include "mode.h"
#include "tools.h"


class Motor {
private:
    uint8_t id_;
    bool allow_EMA_;
public:
    bool initialized_;

private:
    Servo esc_;
    
    uint16_t min_ppm_;
    uint16_t max_ppm_;
    uint16_t reverse_ppm_;

    uint16_t ppm_;
    uint16_t target_ppm_;
    
    int16_t min_vel_;
    int16_t min_vel_rev_;
    int16_t max_vel_;
    int16_t max_vel_rev_;

    uint8_t data_pin_;
    
public:
    Motor(uint8_t id, uint16_t min_ppm, uint16_t max_ppm, uint16_t reverse_ppm, uint16_t min_vel, uint16_t min_vel_rev, uint8_t data_pin);

    static void init();

    void enableEMA();
    void disableEMA();
    static void enableEMAs();
    static void disableEMAs();

    void set_min_ppm();
    void set_reverse_ppm();
    void set_max_ppm();

    static void set_min_ppms();
    static void set_reverse_ppms();
    static void set_max_ppms();

    uint16_t EMA();

    void calc_ppm();
    void write_ppm();
    void set_ppm(int16_t ppm);
    void set_velocity(int16_t vel);
    void set_velocity_percents(int8_t percent);

    static void set_velocity_percents(int8_t percent0, int8_t percent1);
    static void set_bounds(int8_t* bounds);
    static void spin();
};


Motor* motor0;
Motor* motor1;
int8_t* percent_bounds_;
uint64_t t;


Motor::Motor(uint8_t id, uint16_t min_ppm, uint16_t max_ppm, uint16_t reverse_ppm,
             uint16_t min_vel, uint16_t min_vel_rev, uint8_t data_pin) : id_{id},
                                                                         min_ppm_{min_ppm},
                                                                         max_ppm_{max_ppm},
                                                                         reverse_ppm_{reverse_ppm},
                                                                         ppm_{reverse_ppm},
                                                                         target_ppm_{reverse_ppm},
                                                                         min_vel_{min_vel},
                                                                         min_vel_rev_{min_vel_rev},
                                                                         data_pin_{data_pin} {
    max_vel_ = max_ppm_ - reverse_ppm_ - min_vel;
    max_vel_rev_ = min_ppm_ - reverse_ppm_ - min_vel_rev_;
    allow_EMA_ = true;
    initialized_ = false;
    initialized_ = false;
}


void Motor::init() {
    motor0->esc_.attach(motor0->data_pin_, motor0->min_ppm_, motor0->max_ppm_);
    motor1->esc_.attach(motor1->data_pin_, motor1->min_ppm_, motor1->max_ppm_);
    motor0->initialized_ = true;
    motor1->initialized_ = true;
    set_max_ppms();
    delay(2400);
    set_reverse_ppms();
    delay(1000);
}


void Motor::enableEMA() {
    allow_EMA_ = true;
}


void Motor::disableEMA() {
    allow_EMA_ = false;
}


void Motor::enableEMAs() {
    motor0->enableEMA();
    motor1->enableEMA();
}


void Motor::disableEMAs() {
    motor0->disableEMA();
    motor1->disableEMA();
}


void Motor::set_min_ppm() {
    esc_.writeMicroseconds(min_ppm_);
}


void Motor::set_reverse_ppm() {
    esc_.writeMicroseconds(reverse_ppm_);
}


void Motor::set_max_ppm() {
    esc_.writeMicroseconds(max_ppm_);
}


void Motor::set_min_ppms() {
    motor0->set_min_ppm();
    motor1->set_min_ppm();
}


void Motor::set_reverse_ppms() {
    motor0->set_reverse_ppm();
    motor1->set_reverse_ppm();
}


void Motor::set_max_ppms() {
    motor0->set_max_ppm();
    motor1->set_max_ppm();
}


uint16_t Motor::EMA() {
    static const float alpha = 0.075; // 0 -> 100: 500 ms
    
    if (abs(ppm_ - target_ppm_) < 10) {
        return target_ppm_;
    }
    
    return alpha * target_ppm_ + (1 - alpha) * ppm_;
}


void Motor::calc_ppm() {
    if (!allow_EMA_) {
        return;
    }
    
    uint64_t new_t = millis();
    uint64_t dt = new_t - t;
    
    if (dt < 10) {
        return;
    }
    
    // ppm_ = EMA();
    ppm_ = target_ppm_;

    t = new_t;
}


void Motor::write_ppm() {
    esc_.writeMicroseconds(ppm_);
}


void Motor::set_ppm(int16_t ppm) {
    target_ppm_ = constrain(ppm, min_ppm_, max_ppm_);
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

    set_ppm(ppm);
}


void Motor::set_velocity_percents(int8_t percent) {
    if (!allow_EMA_) {
        return;
    }
    
    percent = percent < 0 ? constrain(percent, percent_bounds_[0], percent_bounds_[1]) : constrain(percent, percent_bounds_[2], percent_bounds_[3]);
    int16_t vel = max_vel_ * (percent / 100.0);
    set_velocity(vel);
}


void Motor::set_velocity_percents(int8_t percent0, int8_t percent1) {    
    motor0->set_velocity_percents(percent0);
    motor1->set_velocity_percents(percent1);
}


void Motor::set_bounds(int8_t* bounds) {
    percent_bounds_ = bounds;
}


void Motor::spin() {
    motor0->calc_ppm();
    motor1->calc_ppm();
    motor0->write_ppm();
    motor1->write_ppm();
}


#endif // MOTOR_H
