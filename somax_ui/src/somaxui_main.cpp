//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: somaxui_main.cpp
// DESCRIPTION: entry point for the somax user interface
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

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
#include "libhardware.h"
#include "somaxui_menu.h"
#include "adc50.h"
#include "joystick3d_stick.h"
#include "mcu_motor_controller.h"
#include "videocomposer.h"
#include "inputevent.h"
#include "gimbal.h"
//todo: this is a hack to sends events without event system being complete
#include "inputsource.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//#define KEYPRESS_ENABLED 1

#define SOMAX_NAME "SOMAX-JOY3D"
#define SOMAX_CLASS "SOMAX_HARDWARE-INPUT_HAND-DIRECTIONAL_FORCE_ESTIMATOR"
#define SOMAX_VERSION "1.0.0"
#define SOMAX_RESOURCES "SOMAX-ADC50"


struct termios orig_termios;

#define BUTTON_ANALOG_THRESHOLD  13000

//10 left + 1 center + 10 left
#define NUM_POWER_LEVELS ((MCUMTR_MAX_MOTOR_SPEED*2)+1)
#define NUM_AXIS 3
#define MINMAX_TOLERANCE_FACTOR 200

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifdef KEYPRESS_ENABLED
void reset_terminal_mode();
void set_conio_terminal_mode();
int kbhit();
int getch();
#endif
int run(Joystick * jstick_left, Joystick * jstick_right);
int joy3d_test_adc();
int joy3d_sample_axis(int axis);
int joy3d_sample_button();
void joy3d_changed_cb_run(int axis, char pos);
void joy3d_changed_cb_test_stick(int axis, char pos);
bool joy3d_display_help(int argc, char *argv[]);
int joy3d_open_adc();
int joy3d_open_stick(bool test_mode);
int joy3d_close();

extern int thermalview_init(int argc, char *argv[], suimenu_context main_menu, vidcomp_context video_composer);
extern int gimbalhold_init(int argc, char *argv[], suimenu_context main_menu, vidcomp_context video_composer);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA STRUCTURES
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static Joystick *stick;
static suimenu_context menu;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   somax_data_init(SOMAX_NAME, SOMAX_CLASS, SOMAX_VERSION, SOMAX_RESOURCES);

   if (argc > 1 && !somax_commandline_options_handler(argc, argv))
   {
      if (joy3d_display_help(argc, argv))
         return 0;
   }

   int return_code = 0;

   libhardware_init();

   vidcomp_context vidcomposer = vidcomp_ini_open(VIDCOMP_DISPLAYID_FRAME_PRIMARY);

   return_code = vidcomp_opr_run(vidcomposer, 5);

   if (!return_code)
      menu = suimenu_ini_open(vidcomposer);
   if (!menu)
      return_code = 1;

   if (!return_code)
      return_code = joy3d_open_adc();

   if (!return_code && somax_commandline_has_option(argc, argv, "sample-adc"))
   {
      joy3d_test_adc();
      return_code = 1;
   }

   if (!return_code)
   {
      if (somax_commandline_has_option(argc, argv, "sample-pwr"))
         return_code = joy3d_open_stick(true);
      else
         return_code = joy3d_open_stick(false);
      stick->add_axis(JOY3D_AXIS_X, "AXIS-PAN", false);
      stick->add_axis(JOY3D_AXIS_Y, "AXIS-TILT", true);
      stick->add_axis(JOY3D_AXIS_Z, "AXIS-ROTATE", false);
   }

   if (!return_code)
      gimbal_ini_open();

   if (!return_code)
      return_code = thermalview_init(argc, argv, menu, vidcomposer);

   if (!return_code)
      return_code = gimbalhold_init(argc, argv, menu, vidcomposer);

   if (!return_code)
      suimenu_opr_show(menu, true);

   if (!return_code && somax_commandline_has_option(argc, argv, "calibrate"))
      stick->calibrate();
   else if (!return_code && somax_commandline_has_option(argc, argv, "sample-raw"))
      stick->testsampler();
   else if (!return_code && somax_commandline_has_option(argc, argv, "sample-pwr"))
      stick->run();
   else if (!return_code && somax_commandline_has_option(argc, argv, "run"))
   {
      stick->run();
   }

   if (!return_code)
      return_code = -1;
   joy3d_close();
   libhardware_deinit();
   if (return_code > 1)
      return 0;
   else
      return return_code;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
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
   printf("    sample-pwr - Sample stick motion and report returned power to values stdout\n");
   printf("                 until control-c.\n");
   printf("    sample-adc - Sample anolog to digital convter and report returned values to stdout\n");
   printf("                 until control-c.\n");

   return true;
}

int joy3d_open_adc()
{
   adc50_init(ADC50_INPUT_JOY3D);

   return 0;
}

int joy3d_open_stick(bool test_mode)
{
   stick = new Joystick();
   if (!test_mode)
      stick->connect(joy3d_sample_axis,
                     joy3d_sample_button,
                     joy3d_changed_cb_run,
                     MINMAX_TOLERANCE_FACTOR,
                     NUM_POWER_LEVELS);
   else
      stick->connect(joy3d_sample_axis,
                     joy3d_sample_button,
                     joy3d_changed_cb_test_stick,
                     MINMAX_TOLERANCE_FACTOR,
                     NUM_POWER_LEVELS);

    return 0;
}

int joy3d_close()
{
   return 0;
}

int joy3d_sample_axis(int axis)
{
   if (axis == JOY3D_AXIS_X)
      return adc50_sample_single_end(0);
   else if (axis == JOY3D_AXIS_Y)
      return adc50_sample_single_end(2);
   else if (axis == JOY3D_AXIS_Z)
      return adc50_sample_single_end(1);
   return 0;
}

int joy3d_sample_button()
{
   return adc50_sample_single_end(3) > BUTTON_ANALOG_THRESHOLD;
}

extern void mcu_process_message();
extern void suimenu_inputmixer_injector(input_event event);
void joy3d_changed_cb_run(int axis, char pos)
{
   static int change_count = 0;
   static char last_position;

   if (change_count <= NUM_AXIS)
   {
      change_count++;
      return;
   }

   if (suimenu_inf_showing(menu) && axis == JOY3D_AXIS_Y)
   {
      if (last_position == 0)
      {
         if (pos > last_position)
         {
            input_event ev = inputevt_ini_new(INPUTEVT_EVENTID_UP, INPUTSRC_DEVICEID_JOY3D);
            suimenu_inputmixer_injector(ev);
            //todo: verify the address disposed is not on the stack
            inputevt_ini_dispose(ev);
         }
         else if (pos < last_position)
         {
            input_event ev = inputevt_ini_new(INPUTEVT_EVENTID_DOWN, INPUTSRC_DEVICEID_JOY3D);
            suimenu_inputmixer_injector(ev);
            //todo: verify the address disposed is not on the stack
            inputevt_ini_dispose(ev);
         }
      }
   }
   else if (suimenu_inf_showing(menu) && axis == JOY3D_BUTTON && pos == 0)
   {
      input_event ev = inputevt_ini_new(INPUTEVT_EVENTID_CLICKED, INPUTSRC_DEVICEID_JOY3D);
      suimenu_inputmixer_injector(ev);
      //todo: verify the address disposed is not on the stack
      inputevt_ini_dispose(ev);
      if (!suimenu_inf_showing(menu))
         gimbal_opr_enable_input(true);
   }
   else if (!suimenu_inf_showing(menu) && axis == JOY3D_BUTTON && pos == 0)
   {
      gimbal_opr_enable_input(false);
      input_event ev = inputevt_ini_new(INPUTEVT_EVENTID_CLICKED, INPUTSRC_DEVICEID_JOY3D);
      suimenu_inputmixer_injector(ev);
      //todo: verify the address disposed is not on the stack
      inputevt_ini_dispose(ev);
   }
   else if (!suimenu_inf_showing(menu) && axis != JOY3D_BUTTON)
   {
      input_event ev;
      if (pos < 0)
         ev = inputevt_ini_new(INPUTEVT_EVENTID_DOWN, INPUTSRC_DEVICEID_JOY3D);
      else
         ev = inputevt_ini_new(INPUTEVT_EVENTID_UP, INPUTSRC_DEVICEID_JOY3D);
      //up/down use same field enumerations
      inputevt_cfg_field_i(ev, INPUTEVT_FIELDID_UP_AXIS, axis);
      inputevt_cfg_field_i(ev, INPUTEVT_FIELDID_UP_POWER, pos);
      gimbal_inputmixer_injector(ev);
      //todo: verify the address disposed is not on the stack
      inputevt_ini_dispose(ev);
   }

   last_position = pos;
}

void joy3d_changed_cb_test_stick(int axis, char pos)
{
   static char test_positions[4];

   printf("JOY3D CALLBACK: axis=%1d pos = %+3d   [%+3d,%+3d,%+3d,%1d]\n", axis, pos,
         test_positions[0], test_positions[1], test_positions[2], test_positions[3]);
   test_positions[axis] = pos;
}

int joy3d_test_adc()
{
   printf("Sampling JOY3D:\n");
   while (1)
   {
      int a0 = adc50_sample_single_end(0);
      int a1 = adc50_sample_single_end(1);
      int a2 = adc50_sample_single_end(2);
      int a3 = adc50_sample_single_end(3);

      printf("   JOYPAN[0]: %6d   JOYTILT[2]: %6d  JOYROTATE[1]: %6d JOYB: %6d\r", a0, a1, a2, a3);
      fflush(stdout);
      usleep(1000000 / 20);
   }
   return 0;
}



