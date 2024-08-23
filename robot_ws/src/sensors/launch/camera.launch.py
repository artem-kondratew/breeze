import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():

    # package_name = 'sensors'
    
    # parameters = [os.path.join(get_package_share_directory(package_name), 'config', 'gnss_params.yaml')]

    camera = Node(
        package='v4l2_camera',
        executable='v4l2_camera_node',
        # parameters=[parameters],
        output='screen',
    )

    return LaunchDescription([
        camera,
    ])
