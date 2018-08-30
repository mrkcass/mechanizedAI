
#include "steppermotor.h"
#include "mcu_api.h"
#include "stdlib.h"
#include "string.h"

int do_command (StepperMotor * motor_x, StepperMotor * motor_y, char * cmd, int cmd_len);
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
//    StepperMotor_stop(X_AXIS_MOTOR, false);
//    StepperMotor_powerOn(Y_AXIS_MOTOR);
//    StepperMotor_stop(Y_AXIS_MOTOR, false);
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
   //y-axis motor
   StepperMotor_StepperMotor(Y_AXIS_MOTOR, 44, 46, 48, 14);

   StepperMotor_powerOn(X_AXIS_MOTOR);
   StepperMotor_powerOn(Y_AXIS_MOTOR);
   debug_print(DBG_INFO, "motor controller is up\n");
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
   StepperMotor_stop(X_AXIS_MOTOR, false);

   StepperMotor_powerOn(Y_AXIS_MOTOR);
   StepperMotor_stop(Y_AXIS_MOTOR, false);
}

void motor_controller_power_off()
{
   StepperMotor_stop(X_AXIS_MOTOR, false);
   StepperMotor_powerOff(X_AXIS_MOTOR);

   StepperMotor_stop(Y_AXIS_MOTOR, false);
   StepperMotor_powerOff(Y_AXIS_MOTOR);
}

int do_command (StepperMotor * motor_x, StepperMotor * motor_y, char * cmd)
{
   int processed = 0;
   static int direction;

   #define SPEED_SIGN   7
   #define SPEED_10X    8
   #define SPEED_1X     9


   //speed command format "speed[X|Y|Z][+|-]NN"
   if (!processed && cmd[0] == 's' && cmd[4] == 'd')
   {
      int speed = (10 * (cmd[SPEED_10X] - '0')) + (cmd[SPEED_1X] - '0');
      if (cmd[SPEED_SIGN] == '-')
         speed = -speed;

      if (cmd[5] == 'X')
         StepperMotor_autostep_speed(motor_x, speed);
      else if (cmd[5] == 'Y')
         StepperMotor_autostep_speed(motor_y, speed);
      else if (cmd[5] == 'Z')
         StepperMotor_autostep_speed(motor_x, speed);

      processed = 1;
   }
   else if (!processed && cmd_len == STOP_STR_LEN && !strncmp(cmd, STOP_STR, STOP_STR_LEN))
   {
      debug_print (DBG_INFO, "DO: stop command\n");
      StepperMotor_stop(motor_x, false);
      StepperMotor_stop(motor_y, false);
      processed = 1;
   }
   else if (!processed && cmd_len == RUN_STR_LEN && !strncmp(cmd, RUN_STR, RUN_STR_LEN))
   {
      debug_print (DBG_INFO, "DO: run command\n");
      if (!direction)
      {
         StepperMotor_run(motor_x, StepperMotor_DIR_CW);
         StepperMotor_run(motor_y, StepperMotor_DIR_CW);
         direction = 1;
      }
      else
      {
         StepperMotor_run(motor_x, StepperMotor_DIR_CCW);
         StepperMotor_run(motor_y, StepperMotor_DIR_CCW);
         direction = 0;
      }
      processed = 1;
   }
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


   return processed;
}