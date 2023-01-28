#!/bin/bash

# Get list of interfaces
interfaces=($(ifconfig -a | grep -o '^[^ ]*:' | tr -d :))

# Print interfaces
echo "Available interfaces:"
for i in "${!interfaces[@]}"; do
  printf "%d) %s\n" "$((i+1))" "${interfaces[i]}"
done

# Ask user to select an interface
read -p "Select an interface to put into monitor mode: " selected
selected_interface=${interfaces[selected-1]}

# Put interface into monitor mode
sudo ifconfig $selected_interface down
sudo iwconfig $selected_interface mode monitor
sudo ifconfig $selected_interface up

# Confirm that interface is in monitor mode
iwconfig $selected_interface | grep Mode: | grep Monitor

echo "Interface $selected_interface is now in monitor mode."

