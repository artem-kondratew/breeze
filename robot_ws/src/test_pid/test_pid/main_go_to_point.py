#!/usr/bin/env python3


import rclpy
import sys
import time
from rclpy.node import Node
from threading import Thread

from robot_msgs.msg import UInt8Vector

from locus_test import *
from ICM20948 import *
from simple_pid import PID

# locus = us_nav()
# locus.start()
# time.sleep(1)
# def get_yaw():
#     return 360-(locus.yaw / 3.14 * 180)

imu = IMU()
# print(imu.calibration_gyroscope())

def get_yaw():
    mx, my, yaw, dyaw = imu.get_telemetry()
    # print(dyaw)
    return yaw, dyaw

def normalization_yaw(yaw):
    while yaw > 180:  yaw -= 360
    while yaw < -180: yaw += 360
    return yaw

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

    def __init__(self):
        super().__init__('test_pid')

        self.publisher_ = self.create_publisher(UInt8Vector, '/drive_controller/input', 10)

        self.CMD_IDX = 0
        self.V0_IDX = 1
        self.V1_IDX = 2

        self.v0 = 0
        self.v1 = 0

        self.v0 = 0
        self.v1 = 0
        self.t = time.time()

        self.thread = Thread(target=self.parse)
        self.thread.start()


    def constrain(self, vel):
        vel = +100 if vel > +100 else vel
        vel = -100 if vel < -100 else vel
        return vel

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

    def parse_joystick_state(self, state : list | None): # [c, v0, v1]
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
      
    def rotate(self, rotate_yaw):

        pid_yaw =  PID(0.05, 0.1, 0.001, output_limits = (-0.5, 0.5), setpoint = 0)
        pid_dyaw = PID(50, 20, 0.01, output_limits = (-40, 40))
        
        try:
            for i in range(5):
                yaw, dyaw = get_yaw()
                ang = normalization_yaw(rotate_yaw - yaw)
                print(i)
                while abs(ang) > 2:    
                    yaw, dyaw = get_yaw()
                    
                    ang = normalization_yaw(rotate_yaw - yaw)
                    if abs(ang) < 5: pid_yaw.reset()
                    
                    pid_dyaw.setpoint = pid_yaw(ang)
                    out = int(pid_dyaw(dyaw))
                    
                    self.setVelocities(-out, out)
                    # print(out)
                    time.sleep(0.1)
                # time.sleep(0.5)
                
            self.setVelocities(0, 0)
            time.sleep(0.1)
                
        except Exception as e:  
            self.setVelocities(0, 0)
            time.sleep(0.1)
            print(e)
            exit(0)  
        
    def parse(self):
       # self.initMotors()        
        # time.sleep(5)
        self.setVelocities(0, 0)
        self.rotate(90)
        
        print('END')
        
        
        exit(0)


def main(args=None):
    rclpy.init(args=args)
    motion_detector = RosJoystick()
    rclpy.spin(motion_detector)
    motion_detector.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
