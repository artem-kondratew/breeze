#include "serial/commander.hpp"


#define EXPORT __attribute__((visibility("default")))


extern "C" { // Commander class wrapper

    EXPORT Commander* createCommander() {
        return new Commander();
    }


    EXPORT bool createSerial(Commander* com, char* port, size_t baudrate, size_t cmd_data_size, size_t msg_data_size) {
        return com->createSerial(port, baudrate, cmd_data_size, msg_data_size);
    }


    EXPORT bool connectSerial(Commander* com) {
        return com->connectSerial();
    }


    EXPORT void disconnectSerial(Commander* com) {
        com->disconnectSerial();
    }


    EXPORT void enableSerialVerbose(Commander* com) {
        com->enableSerialVerbose();
    }


    EXPORT void disableSerialVerbose(Commander* com) {
        com->disableSerialVerbose();
    }


    EXPORT void led_on(Commander* com) {
        com->led_on();
    }


    EXPORT void led_off(Commander* com) {
        com->led_off();
    }


    EXPORT void set_min_ppm(Commander* com) {
        com->set_min_ppm();
    }


    EXPORT void set_reverse_ppm(Commander* com) {
        com->set_reverse_ppm();
    }


    EXPORT void set_max_ppm(Commander* com) {
        com->set_max_ppm();
    }


    EXPORT void allow_ema(Commander* com) {
        com->allow_ema();
    }


    EXPORT void set_velocities(Commander* com, int8_t vel0, int8_t vel1) {
        com->set_velocities(vel0, vel1);
    }


    EXPORT void init_motors(Commander* com) {
        com->init_motors();
    }

}
