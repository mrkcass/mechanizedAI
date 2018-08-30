#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "somax.h"
#include "adc50.h"
#include "joystick3d_stick.h"
#include "mcu_motor_controller.h"

//#define KEYPRESS_ENABLED 1

#define SOMAX_NAME "SOMAX-JOY3D"
#define SOMAX_CLASS "SOMAX_HARDWARE-INPUT_HAND-DIRECTIONAL_FORCE_ESTIMATOR"
#define SOMAX_VERSION "1.0.0"
#define SOMAX_RESOURCES "SOMAX-ADC50"


struct termios orig_termios;

#define BUTTON_GPIO  135

//10 left + 1 center + 10 left
#define NUM_POWER_LEVELS ((MCUMTR_MAX_MOTOR_SPEED*2)+1)
#define NUM_AXIS 3
#define MINMAX_TOLERANCE_FACTOR 200

//todo: move this to adcs0
static mraa_i2c_context joy3d_i2c_bus;
static Wire * button;
static Joystick * stick;


#ifdef KEYPRESS_ENABLED
void reset_terminal_mode();
void set_conio_terminal_mode();
int kbhit();
int getch();
#endif
int send_to_mcu(char * msg);
int run(Joystick * jstick_left, Joystick * jstick_right);
int joy3d_test_adc(Joystick jstick);
int joy3d_sample_axis(int axis);
int joy3d_sample_button();
void joy3d_run_changed_cb(int axis, char pos);
void joy3d_test_changed_cb(int axis, char pos);
bool joy3d_display_help(int argc, char *argv[]);
int joy3d_open(bool test_mode);
int joy3d_close();

int main(int argc, char *argv[])
{
   somax_data_init(SOMAX_NAME, SOMAX_CLASS, SOMAX_VERSION, SOMAX_RESOURCES);

   if (argc > 1 && !somax_commandline_options_handler(argc, argv))
   {
      if (joy3d_display_help(argc, argv))
         return 0;
   }

   int had_error = 0;

   if (!had_error)
   {
      if (somax_commandline_has_option(argc, argv, "sample-pwr"))
        had_error = joy3d_open(true);
      else
        had_error = joy3d_open(false);
      stick->add_axis(JOY3D_AXIS_X, "AXIS-PAN", false);
      stick->add_axis(JOY3D_AXIS_Y, "AXIS-TILT", true);
      stick->add_axis(JOY3D_AXIS_Z, "AXIS-ROTATE", false);
   }

   if (!had_error && somax_commandline_has_option(argc, argv, "calibrate"))
     stick->calibrate();
   else if (!had_error && somax_commandline_has_option(argc, argv, "sample-raw"))
      stick->testsampler();
   else
     stick->run();

   if (!joy3d_i2c_bus)
      had_error = -1;
   joy3d_close();
   return had_error;
}

bool joy3d_display_help(int argc, char *argv[])
{
   int show_help = false;
   if (!strcmp(argv[1], "help") ||
       !strcmp(argv[1], "--help") ||
       !strcmp(argv[1], "-h"))
      show_help = true;

   if (!show_help)
      return false;

   printf("\n");
   printf("  options:\n");
   printf("    calibrate  - Sample stick motion to establish a linear power model that\n");
   printf("                 is loaded and mapped to the stick at invocation. The calibration\n");
   printf("                 is saved to file calibration.ini\n");
   printf("    sample-raw - Sample stick motion and report returned raw stick values to stdout\n");
   printf("                 until control-c.\n");
   printf("    sample-pwr - Sample stick motion and report returned power values stdout\n");
   printf("                 until control-c.\n");

   return true;
}

int joy3d_open(bool test_mode)
{
   mraa_init();

   joy3d_i2c_bus = i2c_open(ADC50_I2C_BUS_NUM);
   adc50_init(joy3d_i2c_bus, ADC50_INPUT_JOY3D);
   //joy3d_testsample(joy3d_i2c_bus);

   button = new Wire();
   button->connectDigital(BUTTON_GPIO, WIRE_DIRECTION_IN, WIRE_MODE_PULLDOWN);
   stick = new Joystick();
   if (!test_mode)
      stick->connect(joy3d_sample_axis,
                     joy3d_sample_button,
                     joy3d_run_changed_cb,
                     MINMAX_TOLERANCE_FACTOR,
                     NUM_POWER_LEVELS);
    else
      stick->connect(joy3d_sample_axis,
                     joy3d_sample_button,
                     joy3d_test_changed_cb,
                     MINMAX_TOLERANCE_FACTOR,
                     NUM_POWER_LEVELS);

   return 0;
}

int joy3d_close()
{
   if (joy3d_i2c_bus)
   {
      i2c_close(joy3d_i2c_bus);
   }
   mraa_deinit();
   return 0;
}

int joy3d_sample_axis(int axis)
{
   if (axis == JOY3D_AXIS_X)
      return adc50_sample_single_end(joy3d_i2c_bus, 0);
   else if (axis == JOY3D_AXIS_Y)
      return adc50_sample_single_end(joy3d_i2c_bus, 2);
   else if (axis == JOY3D_AXIS_Z)
      return adc50_sample_single_end(joy3d_i2c_bus, 1);
   return 0;
}

int joy3d_sample_button()
{
   return button->read();
}

void joy3d_run_changed_cb(int axis, char pos)
{
   printf("JOY3D CALLBACK: axis=%d pos = %d\n", axis, pos);
}

void joy3d_test_changed_cb(int axis, char pos)
{
  static char test_positions[4];

  printf("JOY3D CALLBACK: axis=%1d pos = %+3d   [%+3d,%+3d,%+3d,%1d]\n", axis, pos,
         test_positions[0], test_positions[1], test_positions[2], test_positions[3]);
  test_positions[axis] = pos;
}

int joy3d_test_adc(Joystick jstick)
{
   printf("Sampling JOY3D:\n");
   while (1)
   {
      int a0 = adc50_sample_single_end(joy3d_i2c_bus, 0);
      int a1 = adc50_sample_single_end(joy3d_i2c_bus, 1);
      int a2 = adc50_sample_single_end(joy3d_i2c_bus, 2);

      printf("   JOYPAN[0]: %6d   JOYTILT[2]: %6d  JOYROTATE[1]: %6d\r", a0, a1, a2);
      fflush(stdout);
      usleep(1000000 / 20);
   }
   return 0;
}