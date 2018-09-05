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
   dio = mraa_gpio_init_raw(gpio);
   direction = _direction;
   if (dio != NULL)
   {
      pin = gpio;
      mraa_gpio_dir(dio, MRAA_GPIO_OUT);
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
      mraa_gpio_write(dio, level);
}

int Wire::read()
{
   return mraa_gpio_read(dio);
}