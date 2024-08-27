#!/usr/bin/env python3


import rclpy
import time
from rclpy.node import Node
from robot_msgs.msg import Gnss, NavData
from sensor_msgs.msg import NavSatFix
from serial import Serial
from std_msgs.msg import Bool
from std_msgs.msg import Float64
from threading import Thread


class Dispatcher(Node):

    def __init__(self) -> None:
        super().__init__('sensors_dispatcher')

        self.declare_parameters(namespace='', parameters=[('gnss_topic', ''),
                                                          ('magnetometer_topic', ''),
                                                          ('locus_topic', ''),
                                                          ('active_topic', ''),
                                                          ('output_topic', ''),])
        
        self.gnss_topic = self.get_parameter('gnss_topic').value
        self.magnetometer_topic = self.get_parameter('magnetometer_topic').value
        self.locus_topic = self.get_parameter('locus_topic').value
        self.active_topic = self.get_parameter('active_topic').value
        self.output_topic = self.get_parameter('output_topic').value

        self.get_logger().info(f'gnss_topic: {self.gnss_topic}')
        self.get_logger().info(f'magnetometer_topic: {self.magnetometer_topic}')
        self.get_logger().info(f'locus_topic: {self.locus_topic}')
        self.get_logger().info(f'active_topic: {self.active_topic}')
        self.get_logger().info(f'output_topic: {self.output_topic}')
        
        self.gnss_sub_ = self.create_subscription(Gnss, self.gnss_topic, self.gnssCallback, 10)
        self.magnetometer_sub_ = self.create_subscription(Float64, self.magnetometer_topic, self.magnetometerCallback, 10)
        self.locus_sub_ = self.create_subscription(Float64, self.locus_topic, self.locusCallback, 10)

        self.publisher_ = self.create_publisher(NavData, self.get_parameter('output_topic').value, 10)
        self.active_publisher_ = self.create_publisher(Bool, self.get_parameter('active_topic').value, 10)

        self.gnss_ : Gnss = None
        self.magnetometer_ : Float64 = None
        self.locus_ : Float64 = None

        self.timer = self.create_timer(1.0, self.activeCallback)

    def gnssCallback(self, msg : Gnss):
        self.gnss_ = msg
        print(f'gnss: lat = {msg.gnss.latitude}, lon = {msg.gnss.longitude} \
              cog = {msg.cog.data if msg.correct_cog else None}, vel = {msg.vel.data if msg.correct_vel else None}')
        
        msg = NavData()
        msg.gnss = self.gnss_.gnss
        msg.yaw.data = 0.0
        msg.dyaw.data = 0.0
        self.publisher_.publish(msg)

    def magnetometerCallback(self, msg : Float64):
        self.magnetometer_ = msg
        print(f'magnetometer: {msg.data}')

    def locusCallback(self, msg : Float64):
        self.locus_ = msg
        print(f'locus: {msg.data}')

    def activeCallback(self):
        msg = Bool()
        msg.data = True
        self.active_publisher_.publish(msg)
            

def main(args=None):
    rclpy.init(args=args)
    node = Dispatcher()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
