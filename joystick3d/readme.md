**Joystick3D Driver**
#
3D Joystick with two standard axis and a twist style axis at the top of the x - y axis.
#
**Functional Description**

Provides 3 axis of power values and one axis of on / off from a momentary on button.
The power range is application definable and is set to the range -5 through 5 by default.

The driver is agnostic to its source by way of two call backs which must provide the current sample
value from a given axis. The button sampler callback is handled in a similar fashion. A third callback is
used to return the current power level of an axis or button when it changes. A function is also provided to
allow direct query of the values returned by callbacks. The stick can be calibrated and will save
the calibration to file in the directory of invocation. If the file is present the stick will open the file
and load the configuration. Currently, the driver will not load without a calibration file so a default file is
provided. The format is very simple and can be hand editted if desired. The values in the file
shouldn't need much adjustment as this stick has good potentiometers.

The values returned by the stick are extremely linear, and how I have the motor speed mapped
to each, is in the sweet spot. Having 5 forward and 5 reverse levels of speed combined
with a huge throw on all axis', makes the gimbal motion fluid and predectable without the need
for damped harmonic motion compensation.

The minima, maxima and center are modeled as ranges which are derived from calibration. Each axis is
debounced independently. Currently the axis are sampled at 180 samples per second per axis and
debounced with a window size of 2 samples per axis. I have noticed no spurious output what so ever.
#
**Operation**

JOY3D has three axis of control and a single momentary on button. The stick driver
is loaded by a sytemd at boot.

To power on the motor, press the JOY3D button. Power on is power sequenced at takes
1.5 seconds for all three motors. You should here the motor hum when it is powered
on and ready for motion.

Looking down on Somax and from behind the stick.
* Pan the gimbal clockwise by pushing the stick to the right.
* Pan the gimbal counter-clockwise by pushing the stick left.
* Tilt the gimbal up by pulling back on the stick.
* Tilt the gimbal down by pushing the stick forward.
* Rotate the gimbal clockwise when, looking at the back of the gimabl, by twisting the handle clockwise.
* Rotate the gimbal counter-clockwise when looking at the back of the gimabl, by twisting the handle counter-clockwise.

Power the gimbal motors down by again pressing the momentary on button. There
is no delay when powering off the motors and so will happen immediately.
#
**Status**

* September/06/2018 - Motors are fixed, No more brown out! All motors are now enabled.
  A service was also added to start the joystick at boot. The problem was fixed by
  adjusting the voltage going the pan and tilt motors. these motors should only
  be provided with 3.8 volts, they were getting 6 volts. This looked a lot like an
  under volt condition becuase the voltage draw through the motor voltage regulator
  was over heating and shutting down when the watts got to high.
* Sep/05/2018 - Stick is working and is controlling the motors.
  I have non-stick code in the driver that will soon be moved to a soon to be
  created lib and include directory. With current motors, the 18 watt power supply
  is browning out when all three motors are powered up and moving. Before some work
  they were causing a brown out when all three were first powered on. I fixed this
  by adding power sequencing logic to the start up. This included proper power up
  sequence for all motors. I also implemenented a current chopper that powers all lines
  down for n phase changes then drives a single phase change or hold phase to the lines
  before repeating. This is independently adjustable on each axis. Currently the current
  chopper is only enabled when the axis power level is zero. Implementing
  the chopper for a running motor will starve the motor and cause skipping. I think this can be
  somewhat mitigated by a very small amount of chopping combined with another algorithm to
  sequence the periods when a motor is applying power, so that the combination of motors getting
  power at any one moment does not exceed system parameters but still provides enough power for
  effective operation. Switching to full stepping from half stepping cut the power use by almost
  half. This is ok because the motor will be geared at 2 or 3 : 1 to reduce power usage and
  improve gimbal resolution. I'm fairly sure the the pan and tilt motors are under volted because
  when two are running they start skipping. I also noticed that they do not seem to make a
  full rotation when asked to using turn_motor.sh. I think this is also related to
  under voltage as the rotation motor has no such problem when using the same delay.
#
**Hardware**

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

|JH-D400 Pin	| Edison Mini-Breakout Pin  |
|------------- | ------------------------- |
| VIN          | 5.0 POWER                 |
| GND          | GND                       |
| JOYPAN       | I2C1-1, ADC50-A0          |
| JOYROTATE    | I2C1-1, ADC50-A1          |
| JOYTILT      | I2C1-1, ADC50-A2          |
| JOYB         | I2C1-1, ADC50-A3          |
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

	JOYB	PRESSED = HIGH

&nbsp;

|Driver Option	| Description  |
|------------- | ------------------------- |
| run          | start the joystick for normal operation   |
| calibrate    | start calibration function |
| sample-raw   | test the values returned by the adc  |
| sample-pwr   | test converted adc samples as power levels  |
