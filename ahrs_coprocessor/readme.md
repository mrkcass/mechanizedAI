**AHRSCAMD, AHRSFRAME Driver**
#
Dual attitude, heading and reference systems (AHRS)
#
**Status**

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

