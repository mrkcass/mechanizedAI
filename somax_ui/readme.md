## **somax_ui**

Presents a user graphical interface for access to Somax applications.
#

&nbsp;


**Functional Description**

somax_ui is a binary application that is executed and maintained as a systemd
service. somax_ui displays a menu on the frame display (OLEDFRAME).

Available applications:
* **joy3d-control-gimbal** - Demo application to allow 3D joystick control of the camera
  gimbal.
* **joy3d-ahrs-view** - Activates the frame and gimbal attitude, heading and reference systems
  and sends heading, pitch, and roll data to the frame display in graphicalfromat.
  Activates the gimbal and uses 3D joystick for motion control.
* **joy3d-frame-compass** - Activates the frame attitude, heading and reference systems
  and sends heading, pitch, and roll data to the frame display. The frame display
  visualizes the heading in a graphical compass style format.
* **joy3d-gimbal-compass** - Activates the gimbal attitude, heading and reference systems
  and sends heading, pitch, and roll data to the frame display. The frame display
  visualizes the heading in a graphical compass style format. Activates the gimbal
  and uses joystick 3d for motion control.
* **joy3d-thermal-view** - Activates the thermal camera a sends output to the
  frame display. Output is displayed as a 16-bit color heat map. Activates gimbal and
  uses th 3D joystick for motion control.
* **joy3d-thermal-track** - Activates the thermal camera and sends output to the
  frame display. Output is displayed as a 16-bit color heat map with a cross-hair
  for targeting. Activates gimbal and uses th 3D joystick for motion control.
  Allows user to move the gimbal and view thermal data which can be used to position
  an on screen cross-hair rectile on a thermal target. The target can be selected
  by pressing 3D joysitck button 1. When selected the camera gimbal will follow the
  target no matter where it moves and display tracking info such as to bearing
  of frame to target, height of target, direction of target travel, speed of target
  and acceleration of target.

#
**Operation**

Operation is the domain of the linux systemd controller who starts the application
at boot.
#
**Status**

* October 12, 2018 - created project and copied in old 3d joystick functionality.
  Created and installed systemd service to start application at boot. currently,
  only joy3d-control-gimbal and joy3d-thermal-view are enabled. joy3d-gimbal-control
  is ready for use. joy3d-thermal-view is in development and should be ready by
  10/15/2018.

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
