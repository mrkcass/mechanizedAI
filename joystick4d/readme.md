**Joystick3D Driver**
#
4D Joystick using 2 psp thumbsticks and 1 button.
#

| Specs      | Maker       |
| ---------- | -------     |
| Model      | MECHAI-001 	|
| Voltage    | 3.3v (5.0v) |
| Power      | n/a         |
| Dimensions |	50x20x20   |
| Axis       | 4           |
| Button     | 1           |
| Resistance | 5k         |
&nbsp;

**Strapping**

None

&nbsp;


**Pin Map**

|MECHAI-001 Pin| Edison Mini-Breakout Pin |
|------------- | ------------------------- |
| VIN          | 5.0 POWER                 |
| GND          | GND                       |
| JOYPAN       | I2C1	ADC50-A1           |
| JOYROTATE    | I2C1	ADC50-A2           |
| JOYTILTU     | I2C1	ADC50-A3           |
| JOYTILTD     | I2C1	ADC50-A4           |
| JOYB         | GP135                     |
&nbsp;

**Joy-Stick map**

Looking down on the stick with max power to the right and min power to the left

	JOYPAN		STICKLEFT_LEFT		STICKLEFT_CENTER	STICKLEFT_RIGHT
	JOYTILTU 	STICKLEFT_UP		STICKLEFT_CENTER	STICKLEFT_DOWN
	JOYROTATE	STICKRIGHT_LEFT 	STICKRIGHT_CENTER	STICKRIGHT_CLOCK
	JOYTILTL	STICKRIGHT_UP		STICKRIGHT_CENTER	STICKRIGHT_DOWN
&nbsp;

**Stick-ADC Map**

	JOYPAN		STICKLEFT_LEFT  < STICKLEFT_CENTER  < STICKLEFT_RIGHT
	JOYTILTU	STICKLEFT_DOWN  < STICKLEFT_CENTER  < STICKLEFT_UP

	JOYROTATE	STICKRIGHT_LEFT < STICKRIGHT_CENTER  < STICKRIGHT_RIGHT
	JOYTILTL	STICKRIGHT_UP < STICKRIGHT_CENTER  < STICKRIGHT_DOWN

	JOYB	PRESSED = HIGH
