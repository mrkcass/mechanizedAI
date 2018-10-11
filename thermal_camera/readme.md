**THERMCAM Driver**
#
Infrared Thermal 8x8 Sensor Array
#
**Functional Description**
THERMCAM is a 64 pixel thermal camera mounted at the center of the gimbal camera mount.
The camera can detect temperature differences between 0 and 80 degrees celsius with
n +/- 2.5 degree celsius accuracy. The sensor is arranged as an array in a square 8x8
orientation. Data and configuration is transmitted and received over
I2C at a maximum rate of 400 kbits per second. The sensor array is updated at 10 hz.
and can be configured to provide an interupt when motion is detected. Data is returned
ready for use and requires no calibration or interpolation for normal use. The camera can
easily see individual human fingers at shrt ranges and can identify a human at up to
7 meters distance. The sensors tracking ability can be improved with distance calibration
data like that provided by CAMPROX.

The datasheet mentions that the following conditions can adversely affect operational
performance.
*  heat emitting body located close to sensor.
*  warm or cold air flowing across sensor
*  sudden changes in sensor temperature.
*  line of sight interference by transparent objects like glass or acrylic.
*  Dirty or wet sensor.
#
**Operation**

The driver is initialized by systemctl service at boot.

A diagnostic application is provided to verify correct operation and has the following
options.
* info - Display configuration data and exit.
* test-console - Initalize the sensor and then print realtime pixel data to the console
  until ctrl-c is pressed.
* test-oled - Initalize the sensor and then display realtime pixel data converted to 16-bit
  RGB values to the frame oled display until ctrl-c is pressed.
* run-normal - Initialize the sensor and start in normal data coleection mode.
* run-standby - Initialize the sensor and start in standby mode.
* run-sleep - Initialize the sensor and start the sensor in low power sleep mode.
#
**Status**
* **October 7, 2018** - Created initial driver.
* **October 11, 2018** - Driver is talking to the sensor and aquiring pixel data. the
   test-console function is implemented and the test-oled will be in next which should go
   fast. I have intermittent I2C bus hang ups when reading from the sensor.
   I beleive this do to a bad connection. Will track it down tomorrow. also seeing higher
   than expected temperature values, probably a conversion math issue that i have on the
   todo list.
#


| Specs      | Maker       |
| ---------- | -------     |
| Maker      | Panasonic   |
| Model      | AMG 8833    |
| Supplier   | Pesky Products
| Voltage    | 3.3v        |
| Current normal | 4.5mA        |
| Current standby | .8mA        |
| Current sleep | .2mA        |
| Dimensions |	10x18 mm    |
| Bus        | I2C         |
| Frequency  | 400 kbits   |
| Address    | 0x69        |
&nbsp;

**Strapping**

None

&nbsp;


**Pin Map AMG 8833**

|AMG8833 Pin		| Edison Mini-Breakout Pin |
|------------- | ------------------------- |
| VIN          | 3.3 POWER     |
| GND          | GND           |
| SCL          | I2C6	SCL    |
| SDA          | I2C6	SDA    |
| ADO          | NC            |
| 3v3          | NC            |

