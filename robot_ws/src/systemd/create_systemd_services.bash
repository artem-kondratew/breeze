#!/bin/bash

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

sudo cp $SCRIPTPATH/drive_controller/drive_controller.service /etc/systemd/system/
sudo cp $SCRIPTPATH/camera/camera.service /etc/systemd/system/
sudo cp $SCRIPTPATH/nav/nav.service /etc/systemd/system/
sudo cp $SCRIPTPATH/gnss/gnss.service /etc/systemd/system/
sudo cp $SCRIPTPATH/magnetometer/magnetometer.service /etc/systemd/system/

sudo systemctl daemon-reload

sudo systemctl enable --now drive_controller.service
sudo systemctl enable --now camera.service
sudo systemctl enable --now nav.service
sudo systemctl enable --now gnss.service
sudo systemctl enable --now magnetometer.service

systemctl status drive_controller.service
systemctl status camera.service
systemctl status nav.service
systemctl status gnss.service
systemctl status magnetometer.service
