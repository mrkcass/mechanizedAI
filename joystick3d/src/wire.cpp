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

bool Wire::connectDigital(int gpio, int _direction, int mode)
{
   dio = mraa_gpio_init(5);
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
         fprintf(stdout, "mmapped access to gpio %d not supported, falling back to normal mode\n", gpio);
      }
      pin = gpio;
      if (direction == WIRE_DIRECTION_IN)
      {
         int value = mraa_gpio_read(dio);
         printf("Initalized GPIO[%d]: current value = %d\n", gpio, value);
      }
      return true;
   }
   else
   {
      printf("An ERROR occurred initializing joystick button");
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
   return mraa_gpio_read(dio);
}