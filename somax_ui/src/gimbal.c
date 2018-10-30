//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: gimbal.c
// DESCRIPTION: somax interface to camera gimbal
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
#include "mcu_motor_controller.h"
#include "inputevent.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define NUM_AXIS 3

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void gimbal_notify_motor_controller(int motor, int speed);
static void gimbal_inputmixer_observer(input_event event);

extern void mcu_main_init();
extern int mcu_process_message(char *msg, char *reply);

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
static bool gimbal_input_enabled;
static bool gimbal_motors_enabled;
static char POWER_ON_CMD[] = "poweron       \n";
static char POWER_OFF_CMD[] = "poweroff      \n";
static char SPEED_CMD[] = "speed%c%s%02d      \n";
static char ZERO_SPEED[] = "+";
static char NEGATIVE_SPEED[] = "-";
static char POSITIVE_SPEED[] = "+";


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void gimbal_ini_open()
{
   mcu_main_init();
}

void gimbal_ini_close()
{
}

void gimbal_opr_enable_motors(bool enable)
{
   char motor_cmd[16];

   if (!enable && gimbal_motors_enabled)
   {
      printf("JOY3D: stick disabled. powering off motors\n");
      sprintf(motor_cmd, POWER_OFF_CMD);
      gimbal_motors_enabled = false;
      mcu_process_message(motor_cmd, NULL);
   }
   else if (enable && !gimbal_motors_enabled)
   {
      printf("JOY3D: stick enabled. powering on motors\n");
      sprintf(motor_cmd, POWER_ON_CMD);
      mcu_process_message(motor_cmd, NULL);

      gimbal_motors_enabled = true;
   }
}

void gimbal_opr_enable_input(bool enable)
{
   gimbal_input_enabled = enable;
}

void gimbal_inputmixer_injector(input_event event)
{
   gimbal_inputmixer_observer(event);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void gimbal_inputmixer_observer(input_event event)
{
   if (!gimbal_input_enabled)
      return;

   if (inputevt_inf_id(event) == INPUTEVT_EVENTID_UP)
   {
      int motor = inputevt_inf_field_i(event, INPUTEVT_FIELDID_UP_AXIS);
      int speed = inputevt_inf_field_i(event, INPUTEVT_FIELDID_UP_SPEED);
      gimbal_notify_motor_controller(motor, speed);
   }
}

static void gimbal_notify_motor_controller(int motor, int speed)
{
   char motor_cmd[16];
   //char motor_reply[16];
   char motor_id[4] = "XYZ";

   //handle button input
   if (motor == NUM_AXIS)
      return;

   //handle axis input
   if (gimbal_motors_enabled)
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


