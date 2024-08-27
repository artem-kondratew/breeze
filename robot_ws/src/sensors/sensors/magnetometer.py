import adafruit_icm20x
import board
import math
import numpy as np
import os
import time
from rclpy.node import Node
import rclpy
from std_msgs.msg import Float64, Bool
from threading import Thread


class Magnetometer(Node):

    def __init__(self) -> None:
        super().__init__('magnetometer')

        self.declare_parameters(namespace='', parameters=[('output_topic', ''),
                                                          ('active_topic', ''),
                                                          ('alpha', 0.0),])
        self.output_topic = self.get_parameter('output_topic').value
        self.active_topic = self.get_parameter('active_topic').value
        self.alpha = self.get_parameter('alpha').value

        self.get_logger().info(f'output_topic: {self.output_topic}')
        self.get_logger().info(f'active_topic: {self.active_topic}')
        self.get_logger().info(f'alpha: {self.alpha}')

        self.publisher = self.create_publisher(Float64, self.get_parameter('output_topic').value, 10)
        self.active_publisher = self.create_publisher(Bool, self.get_parameter('active_topic').value, 10)

        self.timer = self.create_timer(1.0, self.activeCallback)

        self.icm = adafruit_icm20x.ICM20948(board.I2C(), address=0x68)

        # self.field = (1.0, 1.0, 1.0)
        # self.offset = (0.0, 0.0, 0.0)

        self.A = np.array([[1, 0, 0],
                           [0, 1, 0],
                           [0, 0, 1]])

        self.b = np.array([12.749634601997, -45.7504432405194, 8.70737694940221])
        
        self.vector = None
        self.yaw = None

        self.data = None
        self.is_data = False

        self.thread = Thread(target=self.spin)
        self.thread.start()

    def activeCallback(self):
        msg = Bool()
        msg.data = True
        self.active_publisher.publish(msg)
        
    def low_pass_filter(self, prev_value : float, new_value : float) -> float:
        return self.alpha * prev_value + (1.0 - self.alpha) * new_value

    # def naive_read(self) -> None:
    #     data = self.icm.magnetic
    #     self.magnetic_x = (data[0] - self.offset[0]) / self.field[0]
    #     self.magnetic_y = (data[1] - self.offset[1]) / self.field[1]
    #     self.magnetic_z = (data[2] - self.offset[2]) / self.field[2]

    def read(self):
        self.data = np.array(self.icm.magnetic) if not self.is_data else self.data * self.alpha + np.array(self.icm.magnetic) * (1.0 - self.alpha)
        self.is_data = True
        self.vector = np.dot((self.data - self.b), self.A)

    def spin(self):
        msg = Float64()
        while rclpy.ok():
            self.read()
            self.yaw = math.atan2(self.vector[0], -self.vector[1]) * (180 / math.pi)
            print(self.yaw)
            msg.data = self.yaw
            self.publisher.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    node = Magnetometer()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()
