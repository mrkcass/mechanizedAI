
#include "wire.h"

Wire::Wire()
{
	pin = -1;
	gpio = 0;
}
bool Wire::connect(int edisonPin)
{
	gpio = mraa_gpio_init(edisonPin);
	if (gpio != NULL)
	{
		mraa_gpio_mode(gpio, MRAA_GPIO_STRONG);
		mraa_gpio_dir(gpio, MRAA_GPIO_OUT);
		if (mraa_gpio_use_mmaped(gpio, 1) != MRAA_SUCCESS)
		{
			fprintf(stdout, "mmapped access to gpio %d not supported, falling back to normal mode\n", edisonPin);
		}
		pin = edisonPin;
		return true;
	}
	pin = -1;
	return false;
}

void Wire::write(int level)
{
	mraa_gpio_write(gpio, level);
}