import RPi.GPIO as GPIO
import rclpy
import time
from rclpy.node import Node
from std_msgs.msg import Bool


class ResetArduino(Node):

    def __init__(self):
        super().__init__('reset_arduino')
        self.declare_parameters(namespace='', parameters=[('input_topic', ''),
                                                          ('output_topic', ''),
                                                          ('reset_pin', -1),])
        
        self.input_topic_ = self.get_parameter('input_topic').value
        self.output_topic_ = self.get_parameter('output_topic').value
        self.reset_pin_ = self.get_parameter('reset_pin').value

        self.get_logger().info(f'input_topic: {self.input_topic_}')
        self.get_logger().info(f'output_topic: {self.output_topic_}')
        self.get_logger().info(f'reset_pin: {self.reset_pin_}')

        self.subscription_ = self.create_subscription(Bool, self.get_parameter('input_topic').value, self.callback, 1)
        self.subscription_  # prevent unused variable warning

        self.publisher_ = self.create_publisher(Bool, self.get_parameter('output_topic').value, 10)

        GPIO.setwarnings(False)
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.reset_pin_, GPIO.OUT)
        GPIO.output(18, GPIO.HIGH)

        self.get_logger().info(f'node started')

    def __delete__(self):
        GPIO.cleanup()

    def callback(self, msg):
        if msg.data != True:
            return
        GPIO.output(18, GPIO.LOW)
        time.sleep(0.02)
        GPIO.output(18, GPIO.HIGH)
        self.publisher_.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    motion_detector = ResetArduino()
    rclpy.spin(motion_detector)
    motion_detector.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
