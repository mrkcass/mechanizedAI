**LIDARCAM Driver**
#
ST VL53L1X Time of flight lidar camera
#
**Functional Description**

The ST VL53L1X is a time of flight lidar ranging sensor. The maximun range is 4
meters under ideal conditions. The sensor can return readings at a maximum rate of 50 Hz
with sub-millimeter accuracy. The sensors uses a single vertical cavity surface
emitting laser (VCSEL) at 940nm wave length to project pulses of light onto a target. The
reflected light is captured in a 16x16 array of single photon avalanche diodes (SPAD).
The exposure time is adjustable. Longer exposure times generally produce more
accurate results. The 16x16 array can be configured for the default 27 degree view angle
or varying angle to the minumum of 15 degrees. Data collection can be configured for
single-shot, continuous, or user defined threshold via interrupt. Data is returned
as a single range value in millimeters. The signal rate, ambient signal rate and ranging
status are also reported. The distance mode can be configured to short (less than 135cm
in dark, 135 at ambient light), medium (less then 290 cm in dark, 76 in ambient), and
long range (less then 360 cm, 73 in ambient).


Host-device Communication is over I2C at a maximum rate of 400Kbps. The address is
software configurable even though the process is a bit messy. Additional GPIO
may be connected to allow the device to generate data related interrupts and also
to place the device into low power stanby mode. The sensor input voltage is 3.3 volts
and it's IO voltage is 1.8 volts. The IO voltage can be progamatically adjusted to
work with 3.3 volt IO. Somax operates the device at 3.3v in and 1.8v IO.
#
**Operation**


#
**Status**
* **October 30, 2018** - We're in the pipe 5x5!!!! The sensor is now out of reset
  and sending ranging data. The issue was releated to I2C 16 bit register addressing.
  I was writing the address and data in separate transactions, writing all in a single
  transaction solved the problem. This is the last sensor in proto 3 & proto 4. Somax
  is hardware complete...time for a beer!!
* **October 26, 2018** - Driver is talking to the sensor and is reading values from registers.
  However, The device will not respond to write commands and will not bring itself out of the
  boot sate. The sensor has funny voltage requirements and so I may have blown an internal component
  or perhaps have it stuck in a odd state. order another sensor and will test when it
  arrives.
* **October 25, 2018** - Ported the Sparkfun code to the somax base. using this code
  to drive the chip as the register space is ennormous and would take to long to write
  from scratch.
* **October 20, 2018** - Created initial driver.
#


| Specs      | Maker          |
| ---------- | -------        |
| Maker      | ST             |
| Model      | VL53L1X        |
| Supplier   | Pesky Products |
| Range      | 4 meters       |
| Voltage    | 3.3v           |
| Current normal | 18mA       |
| Current standby | 7mA       |
| Current sleep | 9mA         |
| Dimensions |	10x18 mm       |
| Bus        | I2C            |
| Frequency  | 400 kbits      |
| Address    | 0x29           |
&nbsp;

**Strapping**

None

&nbsp;


**Pin Map VL53L1X**

|VL53L1X Pin		| Edison Mini-Breakout Pin |
|------------- | ------------------------- |
| VIN          | 3.3 POWER          |
| GND          | GND                |
| SCL          | I2C1	SCL  (1v8)  |
| SDA          | I2C1	SDA  (1v8)  |
| XSHUT        | GP77 (1v8)         |
| INTRUPT      | GP82 (1v8)         |

