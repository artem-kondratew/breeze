from joystick import Joystick
from parser import Parser
from udp_socket import Socket


LOCALHOST = '127.0.0.1'
IP = None

SERIAL_PORT =  5005
SERIAL_DATA_SIZE = 5

LINEAR_AXIS = 1
INVERT_LINEAR_AXIS = True
ANGULAR_AXIS = 0
INVERT_ANGULAR_AXIS = True
ALLOW_BUTTON = 0
LED_ON_BUTTON = 3
LED_OFF_BUTTON = 1
MOTOR_INIT_BUTTON = 4


def main():
    serial_socket = Socket(LOCALHOST, SERIAL_PORT, SERIAL_DATA_SIZE)
    serial_socket.disableVerbose()

    joystick = Joystick(LINEAR_AXIS, INVERT_LINEAR_AXIS, ANGULAR_AXIS, INVERT_ANGULAR_AXIS,
                        ALLOW_BUTTON, LED_ON_BUTTON, LED_OFF_BUTTON, MOTOR_INIT_BUTTON)
    joystick.disableVerbose()
    
    parser = Parser(serial_socket)

    while True:
        # parser.blink()
        parser.parse_joystick_state(joystick.spin())
    

if __name__ == '__main__':
    main()
