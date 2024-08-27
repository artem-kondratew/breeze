import math

import rclpy
from rclpy.node import Node
from robot_msgs.msg import NavData, Target, UInt8Vector
from std_msgs.msg import Bool


class Nav(Node):

    def __init__(self):
        super().__init__('gnss')

        self.declare_parameters(namespace='', parameters=[('data_topic', ''),
                                                          ('target_topic', ''),
                                                          ('active_topic', ''),
                                                          ('vel_topic', ''),
                                                          ('teleop_topic', ''),
                                                          ('min_dist', 0.0),
                                                          ('min_angle', 0.0),])
        
        data_topic = self.get_parameter('data_topic').value
        target_topic = self.get_parameter('target_topic').value
        active_topic = self.get_parameter('active_topic').value
        vel_topic = self.get_parameter('vel_topic').value
        teleop_topic = self.get_parameter('teleop_topic').value
        self.min_dist_ = self.get_parameter('min_dist').value
        self.min_angle_ = self.get_parameter('min_angle').value

        self.get_logger().info(f'data_topic: {data_topic}')
        self.get_logger().info(f'target_topic: {target_topic}')
        self.get_logger().info(f'active_topic: {active_topic}')
        self.get_logger().info(f'vel_topic: {vel_topic}')
        self.get_logger().info(f'teleop_topic: {teleop_topic}')
        self.get_logger().info(f'min_dist: {self.min_dist}')
        self.get_logger().info(f'min_angle: {self.min_angle}')

        self.data_sub_ = self.create_subscription(NavData, data_topic, self.dataCallback, 10)
        self.target_sub_ = self.create_subscription(Target, target_topic, self.targetCallback, 10)
        self.teleop_sub_ = self.create_subscription(Bool, teleop_topic, self.teleopCallback, 10)

        self.active_pub_ = self.create_publisher(Bool, active_topic, 10)
        self.vel_pub_ = self.create_publisher(UInt8Vector, vel_topic, 10)

        self.nav_timer_ = self.create_timer(1.0, self.navCallback)
        self.active_timer_ = self.create_timer(1.0, self.activeCallback)

        self.success_ = False

        target_data_ = Target()
        target_data_.angle = 0
        target_data_.gnss.latitude = 43.102647
        target_data_.gnss.longitude = 131.865785
        self.target_init_ = True

        self.data_ = None
        self.target_data_ = None

        self.teleop_ = False

        self.success_ = False

        self.dist_ = None
        self.angle_ = None

        self.max_vel_ = +100
        self.min_vel_ = -100

        self.SET_VELOCITIES = 3

        self.target_id_ = 0
        self.target_init_ = False

    def dataCallback(self, msg : NavData):
        self.data_ = msg
        self.get_logger().info(f'get data')

    def targetCallback(self, msg : Target):
        self.target_data_ = msg
        self.get_logger().info(f'get target')
        self.target_init_ = True

    def teleopCallback(self, msg : Bool):
        self.teleop_ = msg.data
        self.get_logger().info(f'setting teleop to {self.teleop_}')

    def activeCallback(self):
        msg = Bool()
        msg.data = True
        self.active_pub_.publish(msg)

    def deg2rad(self, angle : float) -> float:
        return angle * math.pi / 180.0

    def calcDist(self):
        R = 6371000.0

        lat1 = self.deg2rad(self.data_.gnss.latitude)
        lon1 = self.deg2rad(self.data_.gnss.longitude)

        lat2 = self.deg2rad(self.target_data_.gnss.latitude)
        lon2 = self.deg2rad(self.target_data_.gnss.longitude)

        dlat = abs(lat1 - lat2)
        dlon = abs(lon1 - lon2)

        a = (math.sin(dlat / 2)) ** 2 + math.cos(lat1) * math.cos(lat2) * math.sin(dlon / 2) ** 2
        c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))

        self.dist_ =  R * c
        self.get_logger().info(f'dist = {self.dist_}')

    def calcAngle(self):
        self.angle_ = self.target_data_.angle - self.data_.yaw.data
        self.get_logger().info(f'angle = {self.angle_}')

    def checkDistError(self):
        self.get_logger().info(f'rotate_error: {bool(self.dist_ > self.min_dist_)}')
        return bool(self.dist_ > self.min_dist_)

    def checkAngleError(self):
        self.get_logger().info(f'rotate_error: {bool(abs(self.angle_) > self.min_angle_)}')
        return bool(abs(self.angle_) > self.min_angle_)

    def constrain(self, vel):
        vel = self.max_vel_ if vel > self.max_vel_ else vel
        vel = self.min_vel_ if vel < self.min_vel_ else vel
        return vel

    def setVelocities(self, vel0, vel1):
        v0 = self.constrain(vel0)
        v1 = self.constrain(vel1)
        msg = UInt8Vector()
        v0_sign = 0 if v0 < 0 else 1
        v0 = abs(v0)
        v1_sign = 0 if v1 < 0 else 1
        v1 = abs(v1)
        msg.data = [self.SET_VELOCITIES, v0_sign, v0, v1_sign, v1]
        self.vel_pub_.publish(msg)

    def rotate(self):
        self.get_logger().info(f'ROTATE')

    def move(self):
        self.get_logger().info(f'MOVE')

    def stop(self):
        self.setVelocities(0, 0)
        self.get_logger().info(f'STOP')

    def setSuccess(self):
        self.target_init_ = False
        self.success_ = True
        self.get_logger().info(f'SUCCESS')

    def navCallback(self):
        if not self.data_init_:
            self.get_logger().info(f'NO GNSS DATA')
            return
        if not self.target_init_:
            self.get_logger().info(f'NO TARGET DATA')
            return
        if self.teleop_:
            self.get_logger().info(f'TELEOP MODE')
            return

        self.calcDist()
        self.calcAngle()

        if self.checkDistError():
            self.rotate() # NOT READY YET
            self.move() # NOT READY YET
            return

        if self.checkAngleError():
            self.rotate() # NOT READY YET
            return

        self.setSuccess()
        self.stop()


def main(args=None):
    rclpy.init(args=args)
    node = Nav()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()
