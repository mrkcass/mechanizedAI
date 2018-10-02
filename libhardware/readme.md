**libHardware**
#
C source code library for Somax hardware. Most of the files have a corresponding
hardware test and/or server applications (i.e joystick3d and ahrs_coprocessor).

Somax Sofware Stack

   * Layer 1 - Applications.
      * gimbal_autoscan_and_hold (observation) (planned)
         * A gimbal/camera application to initiate visual auto-scan an environment for a known
         target image. when a possible match is found the camera terminates the
         scan and holds camera heading, pitch and roll on the possible match. in auto-scanning,
         the frame is held stationary while the gimbal(and camera) vertically and horizontally sweeps an
         environment until a target is found, causing the gimbal/camera to cease scanning and
         hold the gimbals heading, pitch, and roll to the target.
      * gimbal_directscan_and_hold (observation) (planned)
         * A gimbal/camera application to visually direct-scan an environment for a known
         target image/object. in direct-scan, the frame is moved by hand while the gimbal remains stationary
         and fixed facing forward until a target is detected, causing the gimbal/camera to
         modulate the heading, pitch, and roll to maintain the gimbal attitude/heading to the target.
      * like_this, like_this_acquire, like_this_teach (all observation) (all planned).
         * applications to use AI to find similar observations. first an observation is
           made (like_this), then an environment is scanned (like_this_acquire) to allow the AI
           to select things like 'this', then the AI selections are reviewed and corrected.
           the process is repeated until the AI no longer requires teaching. this
           action can be used to build a hotdog / not a hotdog detector.
      * display_image (action)
         * application to display a live or static image.
      * send_email (action)
         * application to send an email.
      * call_ems (action)
         * use a connected phone to dial and initiate an AI emergency services video call
           with gimbal control where supported. emergency services need not be 911, fire,
           or police and may be user defined. an AI emergency services call will also
           allow Question and Answer between a user or the AI (if the user is incomunicado).
           The AI will answer simple questions like: who needs assistance. it will also
           allow verbal commands to be issued such as: 'look 90 degrees left' or 'say a phrase'.
      * cause_and_effect_mixer_sequencer.
         * link observations to actions.
   * Layer 2 - Application library, Hardware Diagnostics.
      * libSomaxApplication (planned)
      * test_ahrs_coprocessor.
      * test_joystick3d
   * Layer 3 - IO libray, Hardware services, OS services, Somax services
      * Input / Output Composition, Mixing and Sequencing Library (planned)
      * libSomaxApplication (planned)
      * service_ahrs_coprocessor.
      * service_joystick3d.
   * Layer 4 - Hardware abstraction library, OS abstraction library.
      * libHardware
      * libSomax (planned)
   * Layer 5 - Hardware library. Operating System
      * libHardware.



At present the library does combine some aspects of abstraction with implementation. For
example the RGB OLED driver has a SPI bus implementation yet to be converted to an
abstraction such as i2c_interface. Those who are mixed, like RGB OLED driver, are being
separated as they are modified for use within the application layer.
#
**File Description**

* Analog to Digital Conveter
   * adc50.cpp, adc50.h
* Attitude, Heading and Reference System (AHRS)
   * ahrs.c, ahrs.h - hardware abstraction
   * bno055.h, bno055.c - hardware device
* 3D Joystick
   * joystick3d_stick.c, joystick3d_stick.h - abstraction and device
* Motor Controller
   * atom_motor_controller.c, atom_motor_controller.h - abstraction and device
   * mcu_motor_controller.c, mcu_motor_controller.h - abstraction and device
* RGB LED light strip
   * ledstrip-ws2812b.c, ledstrip-ws2812b.h - device
* Microphone Array
   * micarray-ics52000.c, micarray-ics52000.h - device
* RGB OLED Display
   * ssd1351.c, ssd1351.h - abstraction and device.
* I2C
   * i2c_interface.c, i2c_interface.h - abstraction
* GPIO
   * wire.cpp, wire.h - abstraction
* Somax common values and functions
   * somax.c, somax.h

#
**Building**

* Build the library
   * make
* Delete the target and intermediary build files
   * make clean

#
**Status**
* **September 20, 2018** - Library created. Ported ahrs_coprocessor and joystik3d.

#


