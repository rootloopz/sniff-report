# Sniff-n-Report

Sniff-Report is a tool that monitors WiFi signals sent by devices (cell phones, access points, etc) and has a web interface that is used to control the program and organize the data. 

## Disclaimer

This tool is intended for educational purposes only, showing users the risks that come with easily identifiable SSIDs and leaving devices WiFi powered on when in not in use.

## Getting Started

Currently this project will most likely only run on Linux based machines (to change in the future):

```
git clone https://github.com/rootloopz/sniff-report.git
cd sniff-report
python3 install.py (NOT IMPLEMENTED YET, to run web interface )
```

## Requirements

* Flask
* libtins
* Monitor mode interface

## Acknowledgments

Inspiration, code snippets, etc.
* [libtins C++ Socket/802.11 Library](http://libtins.github.io/)
* [heyyou SSID Sniffer](https://github.com/abzicht/heyyou)
