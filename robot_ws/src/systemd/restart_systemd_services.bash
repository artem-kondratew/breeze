#!/bin/bash

sudo systemctl restart drive_controller.service
sudo systemctl restart video_stream.service
sudo systemctl restart nav.service
sudo systemctl restart gnss.service
sudo systemctl restart magnetometer.service
