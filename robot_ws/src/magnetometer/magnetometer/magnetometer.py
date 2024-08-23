#!/usr/bin/env python3


import math
import rclpy
import time
from rclpy.node import Node
from threading import Thread
from std_msgs.msg import Float64

from .submodules.locus import Locus


class Magnetometer(Node):

    def __init__(self) -> None:
        super().__init__('magnetometer')

        self.declare_parameters(namespace='', parameters=[('output_topic', ''),
                                                          ('locus_port', ''),])
        self.output_topic = self.get_parameter('output_topic').value
        self.locus_port = self.get_parameter('locus_port').value

        self.get_logger().info(f'output_topic: {self.output_topic}')
        self.get_logger().info(f'locus_port: {self.locus_port}')
        
        self.publisher = self.create_publisher(Float64, self.get_parameter('output_topic').value, 10)

        self.locus = Locus(self.locus_port)
        self.calibrateLocus()
        self.locus.start()
        time.sleep(1)

        self.thread = Thread(target=self.spin)
        self.thread.start()

        self.get_logger().info(f'initialized')

    def calibrateLocus(self):
        self.locus.set_yaw_offset(0.0)
        self.get_logger().info(f'locus is calibrated')

    def publish_yaw(self, yaw):
        msg = Float64()
        msg.data = yaw
        self.publisher.publish(msg)

    def spin(self):
        while True:
            yaw = self.locus.get_yaw()
            self.publish_yaw(yaw)
            self.get_logger().info(f'yaw = {yaw * 180 / math.pi}')


def main(args=None):
    rclpy.init(args=args)
    magnetometer = Magnetometer()
    rclpy.spin(magnetometer)
    magnetometer.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
