**Joystick3D Driver**
#
3D Joystick with two standard axis and a twist style axis at the top of the x - y axis.
#

| Specs      | Maker       |
| ---------- | -------     |
| Model      | JH-D400X-R4 |
| Voltage    | 3.3v (5.0v) |
| Power      | n/a         |
| Dimensions |	49x49x95   |
| Axis       | 3           |
| Button     | 1           |
| Resistance | 10k         |
&nbsp;

**Strapping**

None

&nbsp;


**Pin Map**

|JH-D400 Pin	| Edison Mini-Breakout Pin |
|------------- | ------------------------- |
| VIN          | 5.0 POWER                 |
| GND          | GND                       |
| JOYPAN       | I2C1	ADC50-A1           |
| JOYROTATE    | I2C1	ADC50-A2           |
| JOYTILT      | I2C1	ADC50-A3           |
| JOYB         | GP135                     |
&nbsp;

**Joy-Stick map**

Looking down on the stick with max power to the right and min power to the left

	JOYPANSTICK	STICK_LEFT		STICK_CENTER	STICK_RIGHT

	JOYTILT		STICK_UP		STICK_CENTER	STICK_DOWN

	JOYROTATE	STICK_COUNTERCLK	STICK_CENTER	STICK_CLOCK
&nbsp;

**Stick-ADC Map**

	JOYPAN		STICK_LEFT  < STICK_CENTER  < STICK_RIGHT

	JOYTILT		STICK_DOWN  < STICK_CENTER  < STICK_UP

	JOYROTATE	STICK_CLOCK < STICK_CENTER  < STICK_COUNTERCLK

	JOYB	PRESSED = LOW
