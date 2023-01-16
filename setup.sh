#! /usr/bin/bash
echo "In order to run, you need a monitor mode interface, please enter the name of the device below"
read devname
sudo ip link set $devname down
sudo iw $devname set monitor none
sudo ip link set $devname up
