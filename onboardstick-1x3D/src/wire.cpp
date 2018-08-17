#include "mraa/aio.h"

#include "wire.h"

Wire::Wire()
{
	pin = -1;
	dio = 0;
	aio = 0;
}
bool Wire::connectAnalog(int edisonPin)
{
	aio = mraa_aio_init(edisonPin);
	direction = 0;
	return true;
}

bool Wire::connectDigital(int edisonPin, int _direction, int mode)
{
	dio = mraa_gpio_init(edisonPin);
	direction = _direction;
	if (dio != NULL)
	{
		if (direction > WIRE_DIRECTION_OUT)
			mraa_gpio_dir(dio, MRAA_GPIO_OUT);
		else
			mraa_gpio_dir(dio, MRAA_GPIO_IN);

		if (mode == WIRE_MODE_STRONG)
			mraa_gpio_mode(dio, MRAA_GPIO_STRONG);
		else if (mode == WIRE_MODE_PULLUP)
			mraa_gpio_mode(dio, MRAA_GPIO_PULLUP);
		else if (mode == WIRE_MODE_PULLDOWN)
			mraa_gpio_mode(dio, MRAA_GPIO_PULLDOWN);

		if (mraa_gpio_use_mmaped(dio, 1) != MRAA_SUCCESS)
		{
			fprintf(stdout, "mmapped access to gpio %d not supported, falling back to normal mode\n", edisonPin);
		}
		pin = edisonPin;
		return true;
	}
	return false;
}

void Wire::write(int level)
{
	if (direction == WIRE_DIRECTION_OUT)
		mraa_gpio_write(dio, level);
}

int Wire::read()
{
	if (direction == 0)
		return mraa_aio_read(aio);
	else if (direction == WIRE_DIRECTION_IN)
		return mraa_aio_read(aio);
	else
		return -1;
}