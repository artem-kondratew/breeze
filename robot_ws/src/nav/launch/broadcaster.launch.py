import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():

    package_name = 'nav'
    
    parameters = [os.path.join(get_package_share_directory(package_name), 'config', 'params.yaml')]

    broadcaster = Node(
        package=package_name,
        executable='broadcaster',
        parameters=[parameters],
        output='screen',
    )

    return LaunchDescription([
        broadcaster
    ])
