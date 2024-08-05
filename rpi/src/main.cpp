#include <chrono>

#include "serial/serial.hpp"
#include "gui/parser.hpp"
#include "gui/window.hpp"


#define PORT "/dev/ttyUSB0"
#define BAUDRATE 9600
#define CMD_SIZE 3
#define MSG_SIZE 1


int main() {
    Serial* serial = new Serial(PORT, BAUDRATE, CMD_SIZE, MSG_SIZE);

    if (!serial->connect()) {
        exit(1);
    }

    serial->disableVerbose();

    Parser parser(serial);

    Window::initWindow();
    Window::config();

    while (true) {
        int key = getch();
        if (key == KEY_ESC || key == KEY_RETURN) {
            break;
        }
        parser.parse(key);
    }

    Window::finish();
    return 0;
}
