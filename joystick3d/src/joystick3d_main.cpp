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

#define BUTTON_ANALOG_THRESHOLD  13000

//10 left + 1 center + 10 left
#define NUM_POWER_LEVELS ((MCUMTR_MAX_MOTOR_SPEED*2)+1)
#define NUM_AXIS 3
#define MINMAX_TOLERANCE_FACTOR 200

static Joystick * stick;


#ifdef KEYPRESS_ENABLED
void reset_terminal_mode();
void set_conio_terminal_mode();
int kbhit();
int getch();
#endif
int send_to_mcu(char * msg);
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

extern void mcu_main_init();

int main(int argc, char *argv[])
{
   somax_data_init(SOMAX_NAME, SOMAX_CLASS, SOMAX_VERSION, SOMAX_RESOURCES);

   if (argc > 1 && !somax_commandline_options_handler(argc, argv))
   {
      if (joy3d_display_help(argc, argv))
         return 0;
   }

   int return_code = 0;

   mraa_init();

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
      mcu_main_init();

   if (!return_code && somax_commandline_has_option(argc, argv, "calibrate"))
      stick->calibrate();
   else if (!return_code && somax_commandline_has_option(argc, argv, "sample-raw"))
      stick->testsampler();
   else if (!return_code && somax_commandline_has_option(argc, argv, "sample-pwr"))
      stick->run();
   else if (!return_code && somax_commandline_has_option(argc, argv, "run"))
   {
      //motor_controller = new McuMotorController();
      //motor_controller->PowerOn();
      stick->run();
   }

   if (!return_code)
      return_code = -1;
   joy3d_close();
   mraa_deinit();
   if (return_code > 1)
      return 0;
   else
      return return_code;
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

static char POWER_ON_CMD[]  =  "poweron       \n";
static char POWER_OFF_CMD[] =  "poweroff      \n";
static char SPEED_CMD[] = "speed%c%s%02d      \n";
static char STEP_CMD[] = "step%c%s%02d%04d   \n";
static char POSITION_CMD[] = "position   \n";
static char ZERO_SPEED[] = "+";
static char NEGATIVE_SPEED[] = "-";
static char POSITIVE_SPEED[] = "+";
static bool stick_enabled = false;
int open_speed = 1;
#define MOTOR_PAN    0
#define MOTOR_TILT   1
#define MOTOR_ROTATE 2
int mcu_process_message(char *msg, char *reply);
void joy3d_notify_motor_controller(int motor, int speed)
{
   char motor_cmd[16];
   char motor_reply[16];
   char motor_id[4] = "XYZ";

   if (motor == NUM_AXIS && speed != 1)
      return;


   if (motor == NUM_AXIS)
   {
      if (stick_enabled)
      {
         printf("JOY3D: stick disabled. powering off motors\n");
         sprintf(motor_cmd, POWER_OFF_CMD);
         stick_enabled = false;
         mcu_process_message(motor_cmd, NULL);
      }
      else
      {
         printf("JOY3D: stick enabled. powering on motors\n");
         sprintf(motor_cmd, POWER_ON_CMD);
         mcu_process_message(motor_cmd, NULL);
         sprintf(motor_cmd, STEP_CMD, motor_id[MOTOR_TILT], NEGATIVE_SPEED, 1, 50);
         mcu_process_message(motor_cmd, NULL);
         while (1)
         {
            mcu_process_message(POSITION_CMD, motor_reply);
            int pos_x, pos_y, pos_z;
            pos_x = pos_y = pos_z = 0;
            sscanf(motor_reply, "%03d %03d %03d\n", &pos_x, &pos_y, &pos_z);
            if (pos_y == -50)
               break;
         }
         sprintf(motor_cmd, STEP_CMD, motor_id[MOTOR_PAN], NEGATIVE_SPEED, 1, 50);
         mcu_process_message(motor_cmd, NULL);
         while (1)
         {
            mcu_process_message(POSITION_CMD, motor_reply);
            int pos_x, pos_y, pos_z;
            pos_x = pos_y = pos_z = 0;
            sscanf(motor_reply, "%03d %03d %03d\n", &pos_x, &pos_y, &pos_z);
            if (pos_x == -50)
               break;
         }
         sprintf(motor_cmd, STEP_CMD, motor_id[MOTOR_TILT], NEGATIVE_SPEED, 1, 20);
         mcu_process_message(motor_cmd, NULL);
         while (1)
         {
            usleep(50 * U_MILLISECOND);
            mcu_process_message(POSITION_CMD, motor_reply);
            int pos_x, pos_y, pos_z;
            pos_x = pos_y = pos_z = 0;
            sscanf(motor_reply, "%03d %03d %03d\n", &pos_x, &pos_y, &pos_z);
            if (pos_y == -70)
               break;
         }
         stick_enabled = true;
      }

   }
   else if (stick_enabled)
   {
      if (speed == 0)
         sprintf(motor_cmd, SPEED_CMD, motor_id[motor], ZERO_SPEED, 0);
      else if (speed < 0)
         sprintf(motor_cmd, SPEED_CMD, motor_id[motor], NEGATIVE_SPEED, -speed);
      else
         sprintf(motor_cmd, SPEED_CMD, motor_id[motor], POSITIVE_SPEED, speed);
      mcu_process_message(motor_cmd, NULL);
   }
}

extern void mcu_process_message();
void joy3d_changed_cb_run(int axis, char pos)
{
   static int change_count = 0;

   if (change_count < NUM_AXIS)
   {
      change_count++;
      return;
   }
   //printf("JOY3D CALLBACK: axis=%d pos = %d\n", axis, pos);
   //motor_controller->MotorSpeed(axis, pos);
   joy3d_notify_motor_controller(axis, pos);
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