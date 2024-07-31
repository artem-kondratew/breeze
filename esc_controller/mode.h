#ifndef COMMANDS_H
#define COMMANDS_H


#include "motor.h"


#define DEFAULT_MODE    0
#define HALF_POWER_MODE 1

#define MODES_NUM 2

#define VERBOSE true


// max_rev ------ h_bound_rev ++++++ l_bound_rev ------ zero ------ l_bound ++++++ h_bound ------ max


class Mode {
private:
    int8_t bounds_[4];

public:
    Mode() = default;
    Mode(int8_t h_bound_rev, int8_t l_bound_rev, int8_t l_bound, int8_t h_bound);

    static void init();
    static void set_mode(uint8_t idx);
};


static Mode* modes_;


Mode::Mode(int8_t h_bound_rev, int8_t l_bound_rev, int8_t l_bound, int8_t h_bound) {
    bounds_[0] = h_bound_rev;
    bounds_[1] = l_bound_rev;
    bounds_[2] = l_bound;
    bounds_[3] = h_bound;
}


void Mode::init() {
    modes_ = new Mode[MODES_NUM];

    modes_[DEFAULT_MODE] = Mode(-100, 0, 0, 100);
    modes_[HALF_POWER_MODE] = Mode(-50, 0, 0, 50);

    Mode::set_mode(DEFAULT_MODE);
}


void Mode::set_mode(uint8_t idx) {
    
// only for console test
    if (idx == 40) {
        idx = 0;
    }
    if (idx == 41) {
        idx = 1;
    }
// end only for console test

    Motor::set_bounds(modes_[idx].bounds_);
#if VERBOSE
    Serial.print("set mode ");
    Serial.print(idx);
    Serial.print(" with bounds: [");
    Serial.print(modes_[idx].bounds_[0]);
    Serial.print(", ");
    Serial.print(modes_[idx].bounds_[1]);
    Serial.print("] [");
    Serial.print(modes_[idx].bounds_[2]);
    Serial.print(", ");
    Serial.print(modes_[idx].bounds_[3]);
    Serial.print("]");
    Serial.println("");
#endif
}


#endif // COMMANDS_H
