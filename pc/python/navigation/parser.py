from teleop.joystick import Joystick
from kinematics import calcVelocities
import time
from udp.udp_socket import Socket


class Parser:

    LED_ON = 1
    LED_OFF = 2
    SET_VELOCITIES = 3
    INIT_MOTORS = 4

    def __init__(self, socket : Socket) -> None:
        self.socket = socket
        self.v0 = 0
        self.v1 = 0
        self.t = time.time()

    def ledOn(self):
        self.socket.send([Parser.LED_ON, 0, 0, 0, 0])

    def ledOff(self):
        self.socket.send([Parser.LED_OFF, 0, 0, 0, 0])

    def blink(self):
        while True:
            self.ledOn()
            time.sleep(0.5)
            self.ledOff()
            time.sleep(0.5)

    def setVelocities(self, v0, v1):
        cmd = [Parser.SET_VELOCITIES, 0 if v0 < 0 else 1, abs(v0), 0 if v1 < 0 else 1, abs(v1)]
        self.socket.send(cmd)

    def initMotors(self):
        self.socket.send([Parser.INIT_MOTORS, 0, 0, 0, 0])

    def parse_joystick_state(self, state : list | None):
        if state == None:
            return

        self.v0, self.v1 = calcVelocities(state[Joystick.linear_axis_idx], state[Joystick.angular_axis_idx])
        print(self.v0, self.v1)
        self.setVelocities(self.v0, self.v1)

        print(state)

        if state[Joystick.led_on_button_idx]:
            self.ledOn()
        if state[Joystick.led_off_button_idx]:
            self.ledOff()
        if state[Joystick.init_button_idx]:
            self.initMotors()

        self.t = time.time()

    def spin(self):
        if time.time() - self.t > 2:
            self.setVelocities(0, 0)
