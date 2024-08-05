#include <cstdint>
#include <iostream>

#include "serial/commander.hpp"
#include "serial/serial.hpp"
#include "gui/window.hpp"


class Parser {
private:
    inline static int8_t vel0, vel1;
    inline static int prev_cmd = ' ';

    Serial* serial_;
    Commander commander_;

public:
    Parser(Serial* serial) : serial_{serial}, commander_{serial_} {}

    void parse(int key);
};


template <typename T>
T constrain(T data, T min, T max) {
    data = data > max ? max : data;
    data = data < min ? min : data;
    return data;
}


void Parser::parse(int key) {
    bool vels_changed = false;

    if (key == ' ') {
        vel0 = 0;
        vel1 = 0;
        prev_cmd = ' ';
        vels_changed = true;
    }

    if (key == KEY_UP) {
        if (prev_cmd == KEY_UP || prev_cmd == KEY_DOWN) {
            vel0 += 0.1 * commander_.MAX_VEL;
            vel1 += 0.1 * commander_.MAX_VEL;
        }
        else {
            vel0 = 0.1 * commander_.MAX_VEL;
            vel1 = 0.1 * commander_.MAX_VEL;
        }
        prev_cmd = KEY_UP;
        vels_changed = true;
    }

    if (key == KEY_DOWN) {
        if (prev_cmd == KEY_DOWN || prev_cmd == KEY_UP) {
            vel0 -= 0.1 * commander_.MAX_VEL;
            vel1 -= 0.1 * commander_.MAX_VEL;
        }
        else {
            vel0 = -0.1 * commander_.MAX_VEL;
            vel1 = -0.1 * commander_.MAX_VEL;
        }
        prev_cmd = KEY_DOWN;
        vels_changed = true;
    }

    if (key == KEY_LEFT) {
        if (prev_cmd == KEY_LEFT || prev_cmd == KEY_RIGHT) {
            vel0 -= 0.1 * commander_.MAX_VEL;
            vel1 += 0.1 * commander_.MAX_VEL;
        }
        else {
            vel0 = -0.1 * commander_.MAX_VEL;
            vel1 = +0.1 * commander_.MAX_VEL;
        }
        prev_cmd = KEY_LEFT;
        vels_changed = true;
    }

    if (key == KEY_RIGHT) {
        if (prev_cmd == KEY_RIGHT || prev_cmd == KEY_LEFT) {
            vel0 += 0.1 * commander_.MAX_VEL;
            vel1 -= 0.1 * commander_.MAX_VEL;
        }
        else {
            vel0 = +0.1 * commander_.MAX_VEL;
            vel1 = -0.1 * commander_.MAX_VEL;
        }
        prev_cmd = KEY_RIGHT;
        vels_changed = true;
    }

    if (vels_changed) {
        vel0 = constrain(vel0, commander_.MIN_VEL, commander_.MAX_VEL);
        vel1 = constrain(vel1, commander_.MIN_VEL, commander_.MAX_VEL);
        Window::clearVels();
        Window::paintVels(vel0, vel1);
        commander_.set_velocities(vel0, vel1);
        return;
    }

    if (key == 'n' || key == 'N') {
        commander_.led_on();
    }
    if (key == 'f' || key == 'F') {
        commander_.led_off();
    }
    if (key == 'l' || key == 'L') {
        Window::paintSetupPpm(Window::MIN_PPM_SETUP);
        commander_.set_min_ppm();
    }
    if (key == 'r' || key == 'R') {
        Window::paintSetupPpm(Window::REVERSE_PPM_SETUP);
        commander_.set_reverse_ppm();
    }
    if (key == 'h' || key == 'H') {
        Window::paintSetupPpm(Window::MAX_PPM_SETUP);
        commander_.set_max_ppm();
    }
    if (key == 'u' || key == 'U') {
        Window::paintSetupPpm(Window::UNUSED_PPM_SETUP);
        commander_.allow_ema();
    }  
    if (key == 'i' || key == 'I') {
        commander_.init_motors();
    }  
}
