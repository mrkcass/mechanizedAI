
#include "steppermotor.h"
#include "mcu_api.h"
#include "stdlib.h"
#include "string.h"

int do_command (StepperMotor * motor_x, StepperMotor * motor_y, char * cmd);
int nextRunDirectionX = StepperMotor_DIR_CW;
int nextRunDirectionY = StepperMotor_DIR_CW;

#define NUM_MOTORS 2
#define X_AXIS 0
#define Y_AXIS 1
#define X_AXIS_MOTOR (&motors[X_AXIS])
#define Y_AXIS_MOTOR (&motors[Y_AXIS])

StepperMotor motors[2];

// void motor_controller_controlmcu()
// {
//    int len;
//    char buf[32];
//    //StepperMotor * motor = StepperMotor_StepperMotor(2,4,7,8);
//    //StepperMotor * motor = StepperMotor_StepperMotor(128,129,48,49);
//    //x-axis motor
//    StepperMotor_StepperMotor(X_AXIS_MOTOR, 13, 165, 12, 183);
//    //y-axis motor
//    StepperMotor_StepperMotor(Y_AXIS_MOTOR, 44, 46, 48, 14);

//    StepperMotor_powerOn(X_AXIS_MOTOR);
//    StepperMotor_autostep_stop(X_AXIS_MOTOR, false);
//    StepperMotor_powerOn(Y_AXIS_MOTOR);
//    StepperMotor_autostep_stop(Y_AXIS_MOTOR, false);
//    debug_print (DBG_INFO, "Motor is running\n");
//    while (1)
//    {
//       len = host_receive((unsigned char *)buf, 64);
//       while (len > 0)
//       {
//          if (buf[len] == '\n')
//          {
//             buf[len] = '\0';
//             char pbuf[64];
//             mcu_snprintf(pbuf, 64, "host receive:%d: %s\n", len, buf);
//             debug_print (DBG_INFO, pbuf);
//             do_command(X_AXIS_MOTOR, Y_AXIS_MOTOR, buf, len);
//             break;
//          }
//          len--;
//       }
//       StepperMotor_loop(X_AXIS_MOTOR);
//       StepperMotor_loop(Y_AXIS_MOTOR);
//       mcu_delay(1);
//    }

//    StepperMotor_powerOff(X_AXIS_MOTOR);
//    StepperMotor_powerOff(Y_AXIS_MOTOR);
// }

void motor_controller_sliced_init()
{
   StepperMotor motors[2];
   StepperMotor_StepperMotor(X_AXIS_MOTOR, 13, 165, 12, 183);
   StepperMotor_motion_mode(X_AXIS_MOTOR, MOTORMODE_AUTOSTEP);
   //y-axis motor
   StepperMotor_StepperMotor(Y_AXIS_MOTOR, 44, 46, 48, 14);
   StepperMotor_motion_mode(Y_AXIS_MOTOR, MOTORMODE_AUTOSTEP);

   //StepperMotor_powerOn(X_AXIS_MOTOR);
   //StepperMotor_powerOn(Y_AXIS_MOTOR);
   debug_print(DBG_INFO, "motor controller sliced init\n");
}

void motor_controller_sliced_msg(char * msg, char * reply)
{
   do_command(X_AXIS_MOTOR, Y_AXIS_MOTOR, msg);
}

void motor_controller_sliced_run()
{
   StepperMotor_loop(X_AXIS_MOTOR);
   StepperMotor_loop(Y_AXIS_MOTOR);
}

void motor_controller_power_on()
{
   StepperMotor_powerOn(X_AXIS_MOTOR);
   StepperMotor_autostep_stop(X_AXIS_MOTOR, false);

   StepperMotor_powerOn(Y_AXIS_MOTOR);
   StepperMotor_autostep_stop(Y_AXIS_MOTOR, false);
}

void motor_controller_power_off()
{
   StepperMotor_autostep_stop(X_AXIS_MOTOR, false);
   StepperMotor_powerOff(X_AXIS_MOTOR);

   StepperMotor_autostep_stop(Y_AXIS_MOTOR, false);
   StepperMotor_powerOff(Y_AXIS_MOTOR);
}

int do_command (StepperMotor * motor_x, StepperMotor * motor_y, char * cmd)
{
   int processed = 0;
   static int direction;

   #define SPEED_SIGN   6
   #define SPEED_10X    7
   #define SPEED_1X     8

   if (!processed && cmd[0] == 'p' && cmd[1] == 'o' && cmd[2] == 'w' && cmd[6] == 'n')
   {
      //poweron
      motor_controller_power_on(motor_x);
      motor_controller_power_on(motor_y);
      processed = 1;
   }
   else if (!processed && cmd[0] == 'p' && cmd[1] == 'o' && cmd[2] == 'w' && cmd[6] == 'f')
   {
      //poweroff
      motor_controller_power_off(motor_x);
      motor_controller_power_off(motor_y);
      processed = 1;
   }
   else if (!processed && cmd[0] == 's' && cmd[4] == 'd')
   {
      //speed command format "speed[X|Y|Z][+|-]NN"
      int sign = cmd[SPEED_SIGN];
      int x10 = cmd[SPEED_10X] - '0';
      int x1 = cmd[SPEED_1X] - '0';
      int speed = (10 * x10) + x1;
      int signspeed = speed;
      if (sign == '-')
      {
         signspeed = speed * -1;
         mcu_delay(1);
      }

      if (cmd[5] == 'X')
         StepperMotor_autostep_speed(motor_x, signspeed);
      else if (cmd[5] == 'Y')
         StepperMotor_autostep_speed(motor_y, signspeed);
      //else if (cmd[5] == 'Z')
      //   StepperMotor_autostep_speed(motor_z, signspeed);
      processed = 1;
   }
   else if (!processed && cmd[0] == 's' && cmd[1] == 't' && cmd[2] == 'o' && cmd[3] == 'p')
   {
      StepperMotor_autostep_stop(motor_x, false);
      StepperMotor_autostep_stop(motor_y, false);
      processed = 1;
   }
   else if (!processed && cmd[0] == 'r' && cmd[1] == 'u' && cmd[2] == 'n')
   {
      if (!direction)
      {
         StepperMotor_autostep_run(motor_x, StepperMotor_DIR_CW);
         StepperMotor_autostep_run(motor_y, StepperMotor_DIR_CW);
         direction = 1;
      }
      else
      {
         StepperMotor_autostep_run(motor_x, StepperMotor_DIR_CCW);
         StepperMotor_autostep_run(motor_y, StepperMotor_DIR_CCW);
         direction = 0;
      }
      processed = 1;
   }
   /*
   else if (!processed && cmd_len == MODE_STR_LEN && !strncmp(cmd, MODE_STR, MODE_STR_LEN))
   {
      debug_print (DBG_INFO, "DO: mode command\n");
      StepperMotor_toggleStepMode(motor_x);
      StepperMotor_toggleStepMode(motor_y);
      processed = 1;
   }

   else if (!processed && cmd_len == STEPF_STR_LEN && !strncmp(cmd, STEPF_STR, STEPF_STR_LEN))
   {
      debug_print (DBG_INFO, "DO: step forward command\n");
      StepperMotor_step (motor_y, StepperMotor_DIR_CW);
      processed = 1;
   }

   else if (!processed && cmd_len == STEPB_STR_LEN && !strncmp(cmd, STEPB_STR, STEPB_STR_LEN))
   {
      debug_print (DBG_INFO, "DO: step backward command\n");
      StepperMotor_step (motor_y, StepperMotor_DIR_CCW);
      processed = 1;
   }
   */

   return processed;
}