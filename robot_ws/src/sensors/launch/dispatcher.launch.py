import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():

    package_name = 'sensors'
    
    parameters = [os.path.join(get_package_share_directory(package_name), 'config', 'dispatcher_params.yaml')]

    dispatcher = Node(
        package=package_name,
        executable='dispatcher',
        parameters=[parameters],
        output='screen',
    )

    return LaunchDescription([
        dispatcher,
    ])
