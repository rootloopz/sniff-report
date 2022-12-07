#! /usr/bin/bash
sudo ip link set wlx00c0cab05cec down
sudo iw wlx00c0cab05cec set monitor none
sudo ip link set wlx00c0cab05cec up
