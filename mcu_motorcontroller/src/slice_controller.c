
#include <mcu_api.h>
#include <stdlib.h>
#include <string.h>

#include "slice_controller.h"

static void slice_controller_init();
static void slice_controller_distribute_msg(char *cmd, char * reply);
static void slice_controller_run_slices();

int run_slice_controller = 1;

void slice_controller_run()
{
   int len;
   char msg[SLC_MAX_MESSAGE_LEN];
   char reply[SLC_MAX_REPLY_LEN];

   slice_controller_init();

   int run_slice_controller = 1;
   while (run_slice_controller)
   {
      len = host_receive((unsigned char *)msg, SLC_MAX_MESSAGE_LEN);
      while (len > 0)
      {
         if (msg[len] == '\n')
         {
            msg[len] = '\0';
            reply[0] = '\0';
            slice_controller_distribute_msg(msg, reply);
            if (reply[0] != '\0')
               host_send((unsigned char *)reply, SLC_MAX_MESSAGE_LEN);
            break;
         }
         len--;
      }
      slice_controller_run_slices();
      mcu_delay(1);
   }
}

void slice_controller_stop()
{
   run_slice_controller = 0;
}

//main slice hooks
extern int main_sliced_msg(char * msg, char * reply);
//motor controller slice hooks
extern void motor_controller_sliced_init();
extern int motor_controller_sliced_msg(char * msg, char * reply);
extern void motor_controller_sliced_run();

//initialize slice controller and slices
static void slice_controller_init()
{
   debug_print(DBG_INFO, "slice controller up\n");
   motor_controller_sliced_init();
}

//Send out message and get reply.
//Slice code handling messages should be light. The heavy lifting should be done in
//when the slices run function is called.
static void slice_controller_distribute_msg(char * msg, char * reply)
{
   if (!main_sliced_msg(msg, reply))
   if (!motor_controller_sliced_msg(msg, reply))
      ;
}

//Run the slices.
//Each controller is called so that it can can carry out its primary
//tasks.
//
static void slice_controller_run_slices()
{
   motor_controller_sliced_run();
}