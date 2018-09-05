**ADC50 - Analog to Digital Converter Driver**
#
Analog to digial convter with 4 16 bit channels.
#

| Specs      | Maker       |
| ---------- | -------     |
| Model      | TI ADS-1115 |
| Voltage    | 3.3v (5.0v) |
| Power      | .015 mA     |
| Dimensions |	28x17x2     |
| Bus        | I2C         |
| Frequency  | 400 kbits/s |
| Inputs     | 4           |
| Resolution | 16 bits     |
| Max Sample Rate | 860 SPS per channel |

Programable Gain Amplifier (PGA)

|Gain	| 2/3x | 1x	| 2x|	4x|	8x|	16x|
| ---- | ---- | ---- | ---- | ---- | ---- | ---- |
|**Range** | 6.144v | 4.096v | 2.048v | 1.024v | 0.512v | 0.256v |

&nbsp;

**Strapping**

None

&nbsp;


**Pin Map**

|ADC50 Pin     | Edison Mini-Breakout Pin |
|------------- | ------------------------- |
| VIN          | 5.0 POWER                 |
| GND          | GND                       |
| SCL          | I2C1-SCL                  |
| SDA          | I2C1-SDA                  |
| A0           | JOY3D-PAN                 |
| A1    			| JOY3D-TILTL               |
| A2      		| JOY3D-TILTR               |
| A3         	| JOY3D-BTN                 |
&nbsp;


|Driver Option	| Description  |
|------------- | ------------------------- |
| test-sampler | start the adc and return the values sampeled on all channels   |
