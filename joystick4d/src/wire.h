#ifndef __wire_h__
#define __wire_h__

#include "mraa/aio.h"

#define WIRE_DIRECTION_IN  -1
#define WIRE_DIRECTION_OUT 1

#define WIRE_MODE_STRONG 0
#define WIRE_MODE_PULLUP 1
#define WIRE_MODE_PULLDOWN 2


struct Wire
{
	Wire();
	bool connectAnalog(int edisonPin);
	bool connectDigital(int edisonPin, int direction, int mode);
	void write(int level);
	int read();
	
	int pin;
	int direction;
	mraa_aio_context aio;
	mraa_gpio_context dio;
};

#endif
