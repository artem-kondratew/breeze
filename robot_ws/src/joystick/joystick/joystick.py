#!/usr/bin/env python3


import pygame
import rclpy
import sys
import time
from rclpy.node import Node
from threading import Thread

from robot_msgs.msg import UInt8Vector


LINEAR_AXIS = 1
INVERT_LINEAR_AXIS = True
ANGULAR_AXIS = 0
INVERT_ANGULAR_AXIS = True
ALLOW_BUTTON = 0
LED_ON_BUTTON = 3
LED_OFF_BUTTON = 1
MOTOR_INIT_BUTTON = 4
DEC_VEL_BUTTON = 9
INC_VEL_BUTTON = 7
MAX_VEL_BUTTON = 6
MIN_VEL_BUTTON = 8

NOP = 0
LED_ON = 1
LED_OFF = 2
SET_VELOCITIES = 3
INIT_MOTORS = 4


class RosJoystick(Node):

    def initJoystick(self) -> None:
        
        pygame.init()
        pygame.joystick.init()

        if pygame.joystick.get_count() == 0:
            print("No joysticks found")
            sys.exit()

        self.joystick = pygame.joystick.Joystick(0)
        self.joystick.init()

        self.axes = self.joystick.get_numaxes()
        self.buttons = self.joystick.get_numbuttons()
        self.hats = self.joystick.get_numhats()

        if self.linear_axis >= self.axes or self.angular_axis >= self.axes:
            print('wrong axes setup')
            sys.exit()
        if (self.allow_button >= self.buttons or self.led_on_button >= self.buttons or
            self.led_off_button >= self.buttons or self.init_button >= self.buttons or
            self.dec_vel_button >= self.buttons or self.inc_vel_button >= self.buttons or
            self.max_vel_button >= self.buttons or self.min_vel_button >= self.buttons):
            print('wrong buttons setup')
            sys.exit()

        self.CMD_IDX = 0
        self.V0_IDX = 1
        self.V1_IDX = 2

        self.v0 = 0
        self.v1 = 0

        self.allow_move = False
        
        self.verbose = True

        self.max_vel_factor : float = 100
        self.min_vel_factor : float = 10
        self.vel_factor : float = self.min_vel_factor
        self.old_vel_factor : float = self.min_vel_factor

    def __init__(self):
        super().__init__('joystick')
        self.declare_parameters(namespace='', parameters=[('output_topic', ''),])
        
        self.output_topic_ = self.get_parameter('output_topic').value
        self.get_logger().info(f'output_topic: {self.output_topic_}')

        self.publisher_ = self.create_publisher(UInt8Vector, self.get_parameter('output_topic').value, 10)

        self.linear_axis = 1
        self.angular_axis = 0
        self.allow_button = 0
        self.led_on_button = 3
        self.led_off_button = 1
        self.init_button = 4
        self.dec_vel_button = 9
        self.inc_vel_button = 7
        self.max_vel_button = 6
        self.min_vel_button = 8

        self.invert_linear = True
        self.invert_angular = True

        self.initJoystick()
        self.disableVerbose()

        self.v0 = 0
        self.v1 = 0
        self.t = time.time()

        self.thread = Thread(target=self.parse)
        self.thread.start()

    def __delete__(self):
        pygame.quit()

    def constrain(self, vel):
        vel = +1.0 if vel > +1.0 else vel
        vel = -1.0 if vel < -1.0 else vel
        return vel
    
    def mapVel(self, vel, old_min, old_max, new_min, new_max):
        if vel > new_max:
            return new_max
        if vel < new_min:
            return new_min
        return int(vel / (old_max - old_min) * (new_max - new_min))
    
    def mapVels(self, v : tuple, old_min, old_max, new_min, new_max):
        v0 = self.mapVel(v[0], old_min, old_max, new_min, new_max)
        v1 = self.mapVel(v[1], old_min, old_max, new_min, new_max)
        return v0, v1

    def calcVelocities(self, vx, wz):
        v0 = vx - wz
        v1 = vx + wz
        return self.constrain(v0), self.constrain(v1)

    def spin(self):
        state = [NOP, 0, 0] # cmd, v0, v1

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return None
            
            if event.type == pygame.JOYBUTTONDOWN: # change vel_factor
                is_vel_flag_changed = False
                if self.joystick.get_button(self.max_vel_button):
                    self.old_vel_factor = self.vel_factor
                    self.vel_factor = self.max_vel_factor
                    is_vel_flag_changed = True
                    print('set max vel factor')
                if self.joystick.get_button(self.min_vel_button):
                    self.old_vel_factor = self.vel_factor
                    self.vel_factor = self.min_vel_factor
                    is_vel_flag_changed = True
                    print('set min vel factor')
                if self.joystick.get_button(self.dec_vel_button):
                    self.old_vel_factor = self.vel_factor
                    self.vel_factor -= 10
                    is_vel_flag_changed = True
                    print('dec vel factor')
                if self.joystick.get_button(self.inc_vel_button):
                    self.old_vel_factor = self.vel_factor
                    self.vel_factor += 10
                    is_vel_flag_changed = True
                    print('inc vel factor')

                if self.joystick.get_button(self.allow_button):
                    self.allow_move = True
                if self.joystick.get_button(self.led_on_button):
                    state[self.CMD_IDX] = LED_ON
                if self.joystick.get_button(self.led_off_button):
                    state[self.CMD_IDX] = LED_OFF
                if self.joystick.get_button(self.init_button):
                    state[self.CMD_IDX] = INIT_MOTORS

                if is_vel_flag_changed:
                    self.vel_factor = self.max_vel_factor if self.vel_factor > self.max_vel_factor else self.vel_factor
                    self.vel_factor = self.min_vel_factor if self.vel_factor < self.min_vel_factor else self.vel_factor
                    self.v0, self.v1 = self.mapVels((self.v0, self.v1), -self.old_vel_factor, self.old_vel_factor, -self.vel_factor, self.vel_factor)

            if event.type == pygame.JOYBUTTONUP:
                if not self.joystick.get_button(self.allow_button):
                    self.allow_move = False
                
            if event.type == pygame.JOYAXISMOTION:
                v = self.joystick.get_axis(self.linear_axis)
                w = self.joystick.get_axis(self.angular_axis)
                v = round(v if not self.invert_linear else -v, 2)
                w = round(w if not self.invert_angular else -w, 2)
                v = self.calcVelocities(v, w)
                v0, v1 = self.mapVels(v, -1, 1, -self.vel_factor, self.vel_factor)
                self.v0 = v0
                self.v1 = v1
                
            if event.type == pygame.JOYHATMOTION:
                pass

            state[self.V0_IDX] = self.v0
            state[self.V1_IDX] = self.v1

            if not self.allow_move:
                state[self.V0_IDX] = 0
                state[self.V1_IDX] = 0

            print(state, 'max_vel:', self.vel_factor)
            return state

    def enableVerbose(self):
        self.verbose = True

    def disableVerbose(self):
        self.verbose = False

    def ledOn(self):
        msg = UInt8Vector()
        msg.data = [LED_ON, 0, 0, 0, 0]
        self.publisher_.publish(msg)

    def ledOff(self):
        msg = UInt8Vector()
        msg.data = [LED_OFF, 0, 0, 0, 0]
        self.publisher_.publish(msg)

    def blink(self):
        while True:
            self.ledOn()
            time.sleep(0.5)
            self.ledOff()
            time.sleep(0.5)

    def setVelocities(self, v0, v1):
        msg = UInt8Vector()
        msg.data = [SET_VELOCITIES, 0 if v0 < 0 else 1, abs(v0), 0 if v1 < 0 else 1, abs(v1)]
        self.publisher_.publish(msg)

    def initMotors(self):
        msg = UInt8Vector()
        msg.data = [INIT_MOTORS, 0, 0, 0, 0]
        self.publisher_.publish(msg)

    def parse_joystick_state(self, state : list | None):
        if state == None:
            return

        self.v0, self.v1 = state[self.V0_IDX], state[self.V1_IDX]
        self.setVelocities(self.v0, self.v1)

        if state[self.CMD_IDX] == LED_ON:
            self.ledOn()
            print('led on')
        if state[self.CMD_IDX] == LED_OFF:
            self.ledOff()
            print('led off')
        if state[self.CMD_IDX] == INIT_MOTORS:
            self.initMotors()
            print('init motors')

        self.t = time.time()
        
    def parse(self):
        while True:
            # self.blink()
            self.parse_joystick_state(self.spin())

            now = time.time()
            if now - self.t > 1:
                self.setVelocities(self.v0, self.v1)
                print([0, self.v0, self.v1], 't =', now, 'setting vels manually')
                self.t = now


def main(args=None):
    rclpy.init(args=args)
    motion_detector = RosJoystick()
    rclpy.spin(motion_detector)
    motion_detector.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
