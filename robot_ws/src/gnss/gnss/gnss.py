#!/usr/bin/env python3


import rclpy
from pynmeagps import NMEAReader
from rclpy.node import Node
from serial import Serial
from threading import Thread

from sensor_msgs.msg import NavSatFix


class Gnss(Node):

    def __init__(self) -> None:
        super().__init__('gnss')

        self.declare_parameters(namespace='', parameters=[('output_topic', ''),
                                                          ('port', ''),])
        self.output_topic = self.get_parameter('output_topic').value
        self.port = self.get_parameter('port').value

        self.get_logger().info(f'output_topic: {self.output_topic}')
        self.get_logger().info(f'port: {self.port}')
        
        self.publisher = self.create_publisher(NavSatFix, self.get_parameter('output_topic').value, 10)

        self.stream = Serial(self.port, 9600, timeout=3)
        self.reader = NMEAReader(self.stream)
        self.thread = Thread(target=self.parse)
        self.thread.start()

        self.flag = False

    def __delete__(self) -> None:
        self.stream.close()

    def publishCoordinates(self, lat, lon):
        msg = NavSatFix()
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.header.frame_id = 'base_footprint'
        msg.latitude = lat
        msg.longitude = lon
        self.publisher.publish(msg)
        self.get_logger().info(f'latitude: {lat}; longitude: {lon}')

    def parse(self):
        for (_, parsed_data) in self.reader:
            if parsed_data.msgID == "RMC":
                if self.flag:
                    self.publishCoordinates(parsed_data.lat, parsed_data.lon)
                else:
                    self.publishCoordinates(parsed_data.lat, parsed_data.lon)
                self.flag = not self.flag


def main(args=None):
    rclpy.init(args=args)
    gnss = Gnss()
    rclpy.spin(gnss)
    gnss.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
