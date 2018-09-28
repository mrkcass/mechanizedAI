**AHRSCAMD, AHRSFRAME Driver**
#
Dual attitude, heading and reference systems (AHRS)
#
**Functional Description**

AHRSCAMD and AHRSFRAME are independent attitude, heading and reference systems. Unlike
IMU's (inertial measurement units) which return raw acceleration and compass data,
these sensors return compass heading, pitch and roll all in Euler degrees/radians
or quanterions. Each sensor has an automatic calibration mode that is derived
each time the sensors are powered up. Calibration and Euler angle calculation
is possible from raw accelration and compass data using a sensor fusion algorithm.
However, getting the algorithm right and efficient on an embedded platform can waste
valueable developemnt & CPU cycles best used elsewhere.

Each AHRS returns 16 bits of precision on each of the three axis monitored at
100 samples per second.
#
**Operation**

The driver is initialized with an I2C bus and address for each sensors. Once
intialized the sensor will be powered up and ready for operation.

To receive sensor data after initialization, define a callback function as
defined in ahrs.h and pass it as an argument to the run function. The run function is
blocking and will call the callback with updated orientation data when available.

The driver test app can be invoked with one of three options:
* info - Display configuration data for each sensor and exit.
* test - Initalize the sensors and then print realtime orientation data for each sensors
  until ctrl-c is pressed.
* run - Accuate the motors and tilt the camera up until parallel with the frame pitch.
  next, pan camera to face the rear of the frame. next tilt the camera up to an
  angle where a human head might be if seated and controlling the joystick. next, allow
  the camera position to be adjusted by joystick until the joystick button is pressed.
  finally, when the joystick button is pressed record the orientation of the camera
  and maintain it's heading, pitch, and roll no matter how the frame is re-oriented.
#
**Status**
* September 10, 2018 - Sensors are operational and are sending heading, pitch, and roll
  data for both frame and camera sensors. sensors are ready for handoff.
* September 9, 2018 - Both sensors are talking to the host and are reporting register status
  information. Next step is to configure the sensors and read data.
#


| Specs      | Maker       |
| ---------- | -------     |
| Maker      | Bosch       |
| Model      | BNO055      |
| Voltage    | 3.3v        |
| Power      | .3mA        |
| Dimensions |	20x27 mm    |
| Bus        | I2C         |
| Frequency  | 400 kbits   |
| Address    | 0x28        |
&nbsp;

**Strapping**

None

&nbsp;


**Pin Map AHRSCAMD**

|BNO055 Pin		| Edison Mini-Breakout Pin |
|------------- | ------------------------- |
| VIN          | 3.3 POWER     |
| GND          | GND           |
| SCL          | I2C6	SCL    |
| SDA          | I2C6	SDA    |
| RST          | NC            |
| INT          | NC            |
| ADR          | NC            |
| PS0          | NC            |
| PS1          | NC            |

**Pin Map AHRSFRAME**

|BNO055 Pin		| Edison Mini-Breakout Pin |
|------------- | ------------------------- |
| VIN          | 3.3 POWER     |
| GND          | GND           |
| SCL          | I2C1	SCL    |
| SDA          | I2C1	SDA    |
| RST          | NC            |
| INT          | NC            |
| ADR          | NC            |
| PS0          | NC            |
| PS1          | NC            |
&nbsp;

