#!/bin/bash

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

sudo cp $SCRIPTPATH/arduino_serial.service /etc/systemd/system/
sudo cp $SCRIPTPATH/video_stream.service /etc/systemd/system/

sudo systemctl daemon-reload

sudo systemctl enable --now arduino_serial.service
sudo systemctl enable --now video_stream.service

systemctl status arduino_serial.service
systemctl status video_stream.service
