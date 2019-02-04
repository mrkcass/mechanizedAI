**RGBCAM Driver**
#

#
**Functional Description**

RGBCAM is a low resolution RGB camera. Max resolution is 640x480. Images are transferred across a serial UART bus in compressed JPEG format. The camera is capable of 30 frames per second but the UART bus speed 
of 115,200 bits per second limits 160x120 compressed image transfers to about 5 frames per second. Focus is manually adjustable with a range of .5 to 15 meters. The feield of view is 70 degrees.
#
**Operation**
To be Defined

#
**Status**
* **February 3, 2109** - This device took a bit to get working. The 5v input and 3.3v output caused
   me to blow one up though it was mentioned that the camera was 5v tolerent. I also found out that you can brick the camera by attempting to change the baud
   rate which resulted in the second device being rendered useless. The third one is working and I have 
   communiation up and returning configuration information. I also had to move wires around as the Edison
   uses the UART 1 for the FTDI console. This leaves UART 2 which supports hardware flow control 
   though it is not needed in the application. The hardware flow control RTS/CTS lines were previously repurposed for GPIO but this is not allowed when using UART1 so those GPIO lines were moved.
#


| Specs           | Maker                     |
| ----------      | -------                   |
| Maker           | Spinel                    |
| Model           | SC03MPE                   |
| Supplier        | Amazon                    |
| Voltage         | 5.0v(VCC) 3.3v(logic)     |
| Current normal  | 100mA                     |
| Dimensions      | 28x20mm                   |
| Bus             | UART                      |
| Frequency       | 115.2 kbits               |
| Address         | 0x0                       |
| Protocol        | VC0706                    |
| Resolution      | 640x480, 320x240, 160x120 |
| Sensor Pixels   | .3Mp                      |
| Field of View   | 70deg                     |
| Focal length    | 4.3mm                     |
| Min LUX         | 1lux                      |
| Output Format   | JPEG                      |


&nbsp;

**Strapping**

None

&nbsp;


**Pin Map SC03MPE**

|VL53L1X Pin	| Edison Mini-Breakout Pin    |
|------------- | -------------------------   |
| VCC          | 5v POWER                    |
| GND          | GND                         |
| RXD          | UART2-TX  (3v3)             |
| TXD          | UART2-RX	(3v3)             |
| CVBS         |                             |

