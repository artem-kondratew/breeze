#!/bin/bash

sudo cp ./arduino_serial.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --now arduino_serial.service
systemctl status arduino_serial.service
