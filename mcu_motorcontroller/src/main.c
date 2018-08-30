
#include "mcu_api.h"
#include "stdlib.h"
#include "string.h"

extern void slice_controller_run();
run_sliced_msg_distribute(char *cmd, int cmd_len);


void mcu_main()
{
   slice_controller_run();
}


int main_sliced_msg(char * cmd, char * reply)
{
   int processed = 0;

   return processed;
}