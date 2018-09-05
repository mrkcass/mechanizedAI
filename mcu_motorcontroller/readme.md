<span style="font-size:larger;">**MCU_MOTORCTL**</span>

Motor controller written for Edison MCU.

&nbsp;

**Status**

Plenty of bugs still in the code, I stopped work to prove out the algorithm on
the CPU side (atom_motorcontroller.cpp). Think I may stick with the cpu side for
now. When the rest of the bugs are out of the atom driver I will move it to the
quark (mcu).

&nbsp;

**Quark MCU Notes**

* The MCU uses /dev/ttymcu0 to communicate with the atom.
* The sleep function sleeps for 10 milliseconds * the value requested;
* The delay function has excellent real time properties.
* A GPIO is toggled in not less than 10 microseconds. This means that writing a WS2812B RGB LED strip driver in the MCU will most likely be very difficult or impossible. I have some code to experiment but as of yet I have not made an LED twinkle.
* The MCU does not support floating point math.
* The Edsion must be rebooted to restart the MCU using a new image.
* The MCU image is loaded to /lib/firmware and must be named intel_mcu.bin



