#!/bin/bash

source /opt/ros/humble/setup.bash
source /home/pi/breeze/robot_ws/install/setup.bash
ros2 launch sensors gnss.launch.py
