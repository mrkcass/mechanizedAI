<span style="font-size:larger;">**Tools**</span>

This directory contains tools for working within the mechanizedAI framework

&nbsp;

<span style="font-size:larger;">**gpio_map.sh**</span>

Print the current mapping of all gpio pins.


<span style="font-size:larger;">**init_gpio.sh**</span>

Called at power on to initialize GPIO pins.

<span style="font-size:larger;">**turn_motor.sh**</span>

Move one of the 4 camera gimbal motors.
| Option	      | Description  |
|------------- | ------------------------- |
| help         | Show help.
| off          | Shut off power to all motors.
| wifi         | use wifi instead of ethernet over usb. address's is in script.
| p\|u\|r\|l   | select a motor pan, tilt upper, rotate, tilt lower.
| f\|r         | direction forward or reverse.
| NNN          | steps to turn.
| DDD          | delay between step in microseconds.

<span style="font-size:larger;">**edison_cmd.exp**</span>

Tool to automate host <-> communication.
| Option	      | Description  |
|------------- | ------------------------- |
| help   | Show help.
| login  | Login to the edsion module and change to working directory.
| catmcu | Login to Edsion and monitor mcu debug output.
| upload | Upload built mcu image to Edsion module.
| reboot | Execute a controlled reboot in 1 seconds.



