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

Available UI resources:
* **ui_menu** - a menu that displays text menu items in vertical list format.

#
**Operation**

Operation is the domain of the linux systemd controller who starts the application
at boot.
#
**Status**

* October 17,2018 - the thermal camera is operational and is displaying output on
  the frame 1.5" oled. The colors are not correct at the moment but that is for
  tommorrow!
* October 16, 2018 - finished SPI interface. Added a few extras to GPIO interface.
  completed work on videodisplay. started videodisplay_ssd1351 and about half way done
  with the implementation. ssd1351 and the video pipeline will be ready for testing
  by lunch time tomorrow.
* October 15, 2018 - started to add the video display for the ssd1351. realized this
  is a good opportunity to abstract the SPI and GPIO interfaces. the framework for
  GPIO is done as is the implementation. The SPI interface has a framework and the
  implementation is about half way done.
* October 14, 2018 - created framework for input mixer and supporting classes. created video
  composer and supporting classes framework. created the first ui class ui_menu.
* October 12, 2018 - created project and copied in old 3d joystick functionality.
  Created and installed systemd service to start application at boot. currently,
  only joy3d-control-gimbal and joy3d-thermal-view are enabled. joy3d-gimbal-control
  is ready for use. joy3d-thermal-view is in development and should be ready by
  10/15/2018.



