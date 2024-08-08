#include <chrono>

#include "serial/serial.hpp"
#include "serial/socket.hpp"
#include "gui/parser.hpp"
#include "gui/window.hpp"


#define SERIAL_PORT "/dev/ttyUSB0"
#define BAUDRATE 9600
#define SERIAL_CMD_SIZE 3
#define SERIAL_MSG_SIZE 1

#define UDP_PORT 5005
#define UDP_DATA_SIZE 5

#define CMD_IDX     0
#define V0_SIGN_IDX 1
#define V0_IDX      2
#define V1_SIGN_IDX 3
#define V1_IDX      4


int main() {
    Serial* serial = new Serial(SERIAL_PORT, BAUDRATE, SERIAL_CMD_SIZE, SERIAL_MSG_SIZE);

    if (!serial->connect()) {
        exit(1);
    }

    serial->disableVerbose();

    Commander commander(serial);

    Socket socket(UDP_PORT, UDP_DATA_SIZE);
    socket.disableVerbose();

    while (true) {
        std::vector<uint8_t> v = socket.read();
        if (v.size() == 0) {
            continue;
        }
        commander.parse(v[CMD_IDX], v[V0_SIGN_IDX] ? v[V0_IDX] : -v[V0_IDX], v[V1_SIGN_IDX] ? v[V1_IDX] : -v[V1_IDX]);
    }

    return 0;
}
