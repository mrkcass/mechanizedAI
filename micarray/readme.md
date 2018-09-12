**MICARRAY**
#
Four channel MEMS microphone array
#
**Functional Description**

MICARRAY is a 4 channel MEMS, solid state, microphone array. The array is similar to the
array found in the Amazon Alexa except it has 3 fewer channels. Each mic is placed at a
corner of the Somax frame.
#
**Operation**

#
**Status**
* **August 29, 2018** - Mic array is wired and is passing signals electrically. I have verified
operation using a multi-meter while turning signals on and off. A PWM effect can be seen
in all lines when an operation is executed. The same signals return to low voltage when
the operation is terminated. I believe that some additional configuration is needed to
Alsa and pusle audio to get this fully working. Next steps notes are in the pin map.
#


| Specs              | Value       |
| ----------         | -------     |
| Maker              | TDK/InvenSense |
| Model              | ICS-52000 |
| Protocol           | PCM / TDM
| Resolution         | 24 bit per channel |
| Sample Rate        | 48 kHz synchronized per channel |
| Frequency Response | flat 50 Hz - 20 kHz |
| SNR                |  65 dBA |
| Sensitivity        | -26 dB FS |
| Directionality     | Omni
| Power Normal       |  1.4 mA per channel
| Powre Standby      | .02 mA per channel
| Voltage            | 1.8 - 3.3 volts
&nbsp;

**Strapping**

None

&nbsp;


**Pin Map MICARRAY**

|ICS-52000 Pin		| Edison Mini-Breakout Pin |
|------------- | ------------------------- |
| VDD          | 1.8 POWER          |
| GND          | GND                |
| WSO          | mic daisy chain    |
| CONFIG       | NC                 |
| WS           | I2S2-FS            |
| SCK          | I2S2-CLK           |
| SD           | I2S2-RXD           |

&nbsp;

