#!/bin/bash

/bin/bash -c /root/init_gpio.sh

stty -F /dev/ttyMFD1 -crtscts
stty -F /dev/ttyMFD1 115200

rfkill unblock bluetooth
sleep 1
/usr/bin/expect /root/tools/bluetooth.exp
