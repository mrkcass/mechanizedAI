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
      printf("An ERROR occurred initializing digital gpio %d\n", gpio);
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

// struct _gpio {
//     /*@{*/
//     int pin; /**< the pin number, as known to the os. */
//     int phy_pin; /**< pin passed to clean init. -1 none and raw*/
//     int value_fp; /**< the file pointer to the value of the gpio */
//     void (* isr)(void *); /**< the interrupt service request */
//     void *isr_args; /**< args return when interrupt service request triggered */
//     pthread_t thread_id; /**< the isr handler thread id */
//     int isr_value_fp; /**< the isr file pointer on the value */
// #ifndef HAVE_PTHREAD_CANCEL
//     int isr_control_pipe[2]; /**< a pipe used to interrupt the isr from polling the value fd*/
// #endif
//     mraa_boolean_t isr_thread_terminating; /**< is the isr thread being terminated? */
//     mraa_boolean_t owner; /**< If this context originally exported the pin */
//     mraa_result_t (*mmap_write) (mraa_gpio_context dev, int value);
//     int (*mmap_read) (mraa_gpio_context dev);
//     mraa_adv_func_t* advance_func; /**< override function table */
// #if defined(MOCKPLAT)
//     mraa_gpio_dir_t mock_dir; /**< mock direction of the pin */
//     int mock_state; /**< mock state of the pin */
// #endif
//     /*@}*/
// #ifdef PERIPHERALMAN
//     AGpio *bgpio;
// #endif

//     struct _gpio_group *gpio_group;
//     unsigned int num_chips;
//     int *pin_to_gpio_table;
//     unsigned int num_pins;
//     mraa_gpio_events_t events;
//     int *provided_pins;

//     struct _gpio *next;
// };

void Wire::remap(int map_to)
{
   //this is a hack. will remap a previously created mraa gpio
   //to another pin. this may help with using gpio lines that are
   //not always usable with mraa. 
   //specifically i want to use gpio 77.
   int * mraagpio_internal;
   mraagpio_internal = (int*)dio;
   printf ("remap: pin = %d\n", mraagpio_internal[0]);
   printf ("remap: phy_pin = %d\n", mraagpio_internal[1]);
   mraagpio_internal[0] = map_to;
   //mraagpio_internal[1] = map_to;
}