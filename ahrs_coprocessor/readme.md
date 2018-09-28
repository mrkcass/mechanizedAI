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
* September 27, 2018 - Working on getting the ahrs, motor controller, and joystick synchronized. When complete
  the joystick will have new function to calibarate the ahrs. In this mode it will bring itself parallel with the
  frame and then move in a controlled way to get the ahrs calibration settings and then write them
  to file. the file will be loaed with by the ahrs whenever the ahrs is invoked.
* September 26, 2018 - I believe this sensor is not usable in fusion mode for heading, but
  certainly is usable for pitch and roll. The pitch and roll output data is always accurate and
  generally calculated very fast. output of heading data however is prone to error for almost no reason other
  than rolling the sensor. if i calibrate the sensor to be heading, pitch and roll tolerant ([3:3:3:3] calibration),
  it will report accurately for a period of time, longer when not tiled by more than 22 degrees, but if the sensor has not
  rolled for a extended period of time, the sensor will loose it's lock on the heading and will never recover
  the original heading. at this point pitching the sensor up and down for a moment will allow the sensor to
  again stabilize at a new heading. the difference in heaading has no obvious means of calulation. my thinking at the
  moment is to abandon the heading and calculate the orientation based on the raw magnetic field data.
  motor control can move the camera sensor precisely and the position, tilt, and roll can be recorded for each.
  the output at many locations can be saved to file for input to an AI that predicts motor position based on tilt, roll,
  and raw x,y,z magnetic data. it may be possible to use both sensors to improve the accuracy of each by feeding
  both to the ai as one event. when the acccuracy is good then it would be intesting to see how this
  might autoencode to a smaller footprint. might even be possible to subdivide the sphere of data to
  get better accuracy when recombined.
* September 26, 2018 - added quaternion output mode in hopes of getting a better calibration with
   respect to heading. while it improved the accuracy along heading and pitch axis, it had
   no affect on heading, pitch, and roll.
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

