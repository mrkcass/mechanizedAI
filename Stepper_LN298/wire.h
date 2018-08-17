#ifndef __wire_h__
#define __wire_h__

#include "mraa/gpio.h"

struct Wire
{
	Wire();
	bool connect(int edisonPin);
	void write(int level);
	
	int pin;
	mraa_gpio_context gpio;
};

#endif
