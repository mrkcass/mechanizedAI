
#include <stdlib.h>
#include "mcu_api.h"
#include "wire.h"

void Wire_Wire(Wire * wire)
{
	wire->pin = -1;
}

int Wire_connect(Wire * wire, int edisonPin, int direction)
{
	wire->pin = edisonPin;
	wire->direction = direction;
	gpio_setup(wire->pin, wire->direction);

	return 1;
}

void Wire_write(Wire * wire, int level)
{
	gpio_write(wire->pin, level);
}