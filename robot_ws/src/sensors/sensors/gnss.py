#!/usr/bin/env python3


import rclpy
import time
from pynmeagps import NMEAReader
from rclpy.node import Node
from serial import Serial
from threading import Thread

from robot_msgs.msg import Gnss as GnssMsg
from std_msgs.msg import Bool


class Gnss(Node):

    def __init__(self) -> None:
        super().__init__('gnss')

        self.declare_parameters(namespace='', parameters=[('output_topic', ''),
                                                          ('active_topic', ''),
                                                          ('port', ''),])
        
        self.output_topic = self.get_parameter('output_topic').value
        self.service_topic = self.get_parameter('active_topic').value
        self.port = self.get_parameter('port').value

        self.get_logger().info(f'output_topic: {self.output_topic}')
        self.get_logger().info(f'active_topic: {self.service_topic}')
        self.get_logger().info(f'port: {self.port}')
        
        self.publisher = self.create_publisher(GnssMsg, self.get_parameter('output_topic').value, 10)
        self.active_publisher = self.create_publisher(Bool, self.get_parameter('active_topic').value, 10)

        self.stream = Serial(self.port, 9600, timeout=3)
        self.reader = NMEAReader(self.stream)
        self.thread = Thread(target=self.parse)
        self.thread.start()

        self.lat = None
        self.lon = None
        self.cog = None

        self.cnt = 0

        self.flag = False
        self.file = open(f'/home/pi/breeze/robot_ws/gnss_{time.strftime("%H_%M_%S")}.log', 'w')

        self.timer = self.create_timer(1.0, self.activeCallback)

    def activeCallback(self):
        msg = Bool()
        msg.data = True
        self.active_publisher.publish(msg)

    def __delete__(self) -> None:
        self.stream.close()

    def publishData(self):
        msg = GnssMsg()
        msg.gnss.header.stamp = self.get_clock().now().to_msg()
        msg.gnss.header.frame_id = 'base_footprint'
        msg.gnss.latitude = self.lat
        msg.gnss.longitude = self.lon
        msg.correct_cog.data = True if self.cog else False
        msg.correct_vel.data = True if self.vel else False
        msg.cog.data = self.cog if msg.correct_cog else 0.0
        msg.vel.data = self.vel if msg.correct_vel else 0.0
        self.publisher.publish(msg)

    def parse(self):
        while True:
            for (_, parsed_data) in self.reader:
                if parsed_data == None:
                    continue
                if parsed_data.msgID == "RMC":
                    lat, lon, self.cog, self.vel = parsed_data.lat, parsed_data.lon, parsed_data.cog, parsed_data.spd
                    if not lat or not lon:
                        continue
                    self.lat = lat if self.lat == None else self.alpha * self.lat + (1.0 - self.alpha) * lat
                    self.lon = lon if self.lon == None else self.alpha * self.lon + (1.0 - self.alpha) * lon
                    self.publishData()
                    self.get_logger().info(f'latitude: {self.lat}, longitude: {self.lon}, cog: {self.cog}')
                    self.file.write(f'{self.lat}, {self.lon}, {time.strftime("%c")}, {self.cnt}\n')
                    self.lat = None
                    self.lon = None
                    self.cog = None
                    self.vel = None
            

def main(args=None):
    rclpy.init(args=args)
    gnss = Gnss()
    rclpy.spin(gnss)
    gnss.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
