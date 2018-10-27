**LIDARCAM Driver**
#
ST VL53L1X Time of flight lidar camera
#
**Functional Description**

#
**Operation**


#
**Status**
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
| Current normal |            |
| Current standby |           |
| Current sleep |             |
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
| XSHUT        | GP77               |
| INTRUPT      | GP82               |

