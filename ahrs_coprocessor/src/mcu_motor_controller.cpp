
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mcu_motor_controller.h"

static char POWER_ON_CMD[]  =  "poweron       \n";
static char POWER_OFF_CMD[] =  "poweroff      \n";
static char SPEED_CMD[] = "speed%c%s%02d      \n";

static char ZERO_SPEED[]      = "+";
static char NEGATIVE_SPEED[]  = "-";
static char POSITIVE_SPEED[]  = "+";

int mcu_send(char *msg);

McuMotorController::McuMotorController()
{

}

McuMotorController::~McuMotorController()
{
}

void McuMotorController::PowerOn()
{
   mcu_send(POWER_ON_CMD);
}

void McuMotorController::PowerOff()
{
   mcu_send(POWER_OFF_CMD);
}

void McuMotorController::MotorSpeed(int motor, int speed)
{
   char speed_cmd[16];
   char motor_id[4] = "XYZ";

   if (motor < 0 || motor > MCUMTR_NUM_MOTORS)
      return;
   if (speed < -MCUMTR_MAX_MOTOR_SPEED || speed > MCUMTR_MAX_MOTOR_SPEED)
      return;

   if (speed == 0)
      sprintf(speed_cmd, SPEED_CMD, motor_id[motor], ZERO_SPEED, 0);
   else if (speed < 0)
      sprintf(speed_cmd, SPEED_CMD, motor_id[motor], NEGATIVE_SPEED, -speed);
   else
      sprintf(speed_cmd, SPEED_CMD, motor_id[motor], POSITIVE_SPEED, speed);

   printf("McuMotorController::MotorSpeed: %s\n", speed_cmd);
   mcu_send(speed_cmd);
}

int mcu_send(char *msg)
{
   int msg_len = strlen(msg);
   int mcu_fd = open("/dev/ttymcu0", O_RDWR | O_NOCTTY);
   if (mcu_fd == -1)
   {
      printf("open ttymcu0 failed!\n");
      return 1;
   }
   else
   {
      write(mcu_fd, msg, msg_len);
      close(mcu_fd);
   }
   return 0;
}

int mcu_fetch(char *msg, int capacity)
{
   int mcu_fd = open("/dev/ttymcu0", O_RDWR | O_NOCTTY);
   if (mcu_fd == -1)
   {
      printf("open ttymcu0 failed!\n");
      return 1;
   }
   else
   {
      read(mcu_fd, msg, capacity);
      close(mcu_fd);
   }
   return 0;
}