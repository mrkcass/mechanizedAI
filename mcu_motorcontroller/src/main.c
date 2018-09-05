
#include "mcu_api.h"
#include "stdlib.h"
#include "string.h"

void logger(char * entry)
{
   char b2[70];
   mcu_snprintf(b2, 70, "%s\n", entry);
   debug_print(DBG_INFO, b2);
}

void loggerS(char *format, char * string)
{
   char b[70];
   mcu_snprintf(b, 70, "%s\n", format);
   debug_print(DBG_INFO, b, string);
}

void loggerI(char *format, int i)
{
   char b[70];
   mcu_snprintf(b, 70, "%s\n", format);
   debug_print(DBG_INFO, b, i);
}
void loggerU(char *format, unsigned long ul)
{
   char b[70];
   mcu_snprintf(b, 70, "%s\n", format);
   debug_print(DBG_INFO, b, ul);
}

void loggerSI(char *format, char *c, int i)
{
   char b[70];
   mcu_snprintf(b, 70, "%s\n", format);
   debug_print(DBG_INFO, b, c, i);
}

int loggerMotorTrigger = 0;
void loggerMT(int enableM)
{
   loggerMotorTrigger = enableM;
}
static int debug_counter;
void loggerM(int motorid, char * entry)
{
   if (motorid == 0)
   {
      if (debug_counter == 10000 || loggerMotorTrigger == 1)
      {
         logger(entry);
         debug_counter = 0;
      }
      debug_counter++;
   }
}

void loggerMI(int motorid, char *entry, int i)
{
   if (motorid == 0 || loggerMotorTrigger == 1)
   {
      if (debug_counter == 10000)
      {
         loggerI(entry, i);
         debug_counter = 0;
      }
      debug_counter++;
   }
}
void loggerMU(int motorid, char *entry, unsigned long ul)
{
   if (motorid == 0)
   {
      if (debug_counter == 10000 || loggerMotorTrigger == 1)
      {
         loggerU(entry, ul);
         debug_counter = 0;
      }
      debug_counter++;
   }
}

#define NUM_MOTORS   3
#define MIN_PHASE    0
#define MAX_PHASE    3
#define NUM_PHASE    4
#define CLOCK_DELAY  100

#define NUM_POWER_LEVELS   11
#define MOTOR_POWER_0      5
#define MOTOR_PAN          0
#define MOTOR_TILT         1
#define MOTOR_ROTATE       2

extern void slice_controller_run();
extern void run_sliced_msg_distribute(char *msg, int msg_len);

int motor_speed[NUM_MOTORS] = {MOTOR_POWER_0, MOTOR_POWER_0, MOTOR_POWER_0};
int motor_phase[NUM_MOTORS] = {0,0,0};
int motor_speed_delay[NUM_POWER_LEVELS][NUM_MOTORS] =
{
   { 5000,  5000,  5000},
   {10000, 10000, 10000},
   {20000, 20000, 20000},
   {30000, 30000, 30000},
   {40000, 40000, 40000},
   { 2000,  2000,  2000},
   {40000, 40000, 40000},
   {30000, 30000, 30000},
   {20000, 20000, 20000},
   {10000, 10000, 10000},
   { 5000,  5000,  5000},
};
unsigned long motor_countdowntimer[NUM_MOTORS] = {2000, 2000, 2000};
unsigned long motor_countdowntimes_lasttime[NUM_MOTORS] = {0, 0, 0};

void init_cpu_to_mx1508()
{
   gpio_setup(13, 1);
   gpio_setup(165, 1);
   gpio_setup(12, 1);
   gpio_setup(183, 1);

   gpio_setup(44, 1);
   gpio_setup(46, 1);
   gpio_setup(48, 1);
   gpio_setup(14, 1);

   gpio_setup(134, 1);
   gpio_setup(45, 1);
   gpio_setup(47, 1);
   gpio_setup(49, 1);
   motor_countdowntimes_lasttime[MOTOR_PAN] = time_us();
   //motor_countdowntimer[MOTOR_PAN] = motor_speed_delay[MOTOR_POWER_0][MOTOR_PAN];
   motor_countdowntimes_lasttime[MOTOR_TILT] = time_us();
   //motor_countdowntimer[MOTOR_TILT] = motor_speed_delay[MOTOR_POWER_0][MOTOR_TILT];
   motor_countdowntimes_lasttime[MOTOR_ROTATE] = time_us();
   //motor_countdowntimer[MOTOR_ROTATE] = motor_speed_delay[MOTOR_POWER_0][MOTOR_ROTATE];
}

void mx1508_to_motor(int phase, int a_pos, int a_neg, int b_pos, int b_neg)
{
   int low = 0, high = 1;

   if (phase == 0)
   {
      gpio_write(a_pos, high);
      gpio_write(a_neg, low);
      gpio_write(b_pos, low);
      gpio_write(b_neg, low);
   }

   if (phase == 1)
   {
      gpio_write(a_pos, low);
      gpio_write(a_neg, high);
      gpio_write(b_pos, low);
      gpio_write(b_neg, low);
   }

   if (phase == 2)
   {
      gpio_write(a_pos, low);
      gpio_write(a_neg, low);
      gpio_write(b_pos, low);
      gpio_write(b_neg, low);
   }

   if (phase == 3)
   {
      gpio_write(a_pos, low);
      gpio_write(a_neg, low);
      gpio_write(b_pos, low);
      gpio_write(b_neg, high);
   }
}

void cpu_to_mx1508(int motor_id)
{
   int in1, in2, in3, in4;

   if (motor_id == MOTOR_PAN)
   {
      in1 = 13;
      in2 = 165;
      in3 = 12;
      in4 = 183;
   }
   if (motor_id == MOTOR_TILT)
   {
      in1 = 44;
      in2 = 46;
      in3 = 48;
      in4 = 14;
   }
   if (motor_id == MOTOR_ROTATE)
   {
      in1 = 134;
      in2 = 45;
      in3 = 47;
      in4 = 49;
   }

   mx1508_to_motor(motor_phase[motor_id], in1, in2, in3, in4);
}

int cmp_phase = 0;
void change_motor_phase(int motor_id)
{
   //loggerM(0, "change_motor phase1");
   if (motor_speed[motor_id] > 0)
   {
      //motor_phase[motor_id] = motor_phase[motor_id]+1;
      if (motor_phase[motor_id] >= NUM_PHASE)
         motor_phase[motor_id] = 0;
   }
   else if (motor_speed[motor_id] < 0)
   {
   //    motor_phase[motor_id]--;
      if (motor_phase[motor_id] < 0)
          motor_phase[motor_id] = NUM_PHASE-1;
   }
   //loggerM(0, "change_motor phase2");
   cpu_to_mx1508(motor_id);
}

void move_motor(int motor_id)
{
   unsigned long time_now = time_us();
   unsigned long elapsed = time_now - motor_countdowntimes_lasttime[motor_id];
   motor_countdowntimes_lasttime[motor_id] = time_now;
   if (motor_id == MOTOR_PAN)
   {
      int low = 0, high = 1;

      if (cmp_phase == 0)
      {
         gpio_write(13, high);
         gpio_write(165, low);
         gpio_write(12, low);
         gpio_write(183, low);
      }

      if (cmp_phase == 1)
      {
         gpio_write(13,  low);
         gpio_write(165, low);
         gpio_write(12,  high);
         gpio_write(183, low);
      }

      if (cmp_phase == 2)
      {
         gpio_write(13,  low);
         gpio_write(165, high);
         gpio_write(12,  low);
         gpio_write(183, low);
      }

      if (cmp_phase == 3)
      {
         gpio_write(13,  low);
         gpio_write(165, low);
         gpio_write(12,  low);
         gpio_write(183, high);
      }

      cmp_phase++;
      if(cmp_phase >= 4)
         cmp_phase = 0;
   }
   return;
   //if (motor_speed[motor_id] >= 1)

   //   loggerMT(1);
   //loggerM(0, "move_motor");
   //if (motor_countdowntimer[motor_id])
   //{

      if (motor_countdowntimer[motor_id] <= elapsed )
      {
         motor_countdowntimer[motor_id] = motor_speed_delay[motor_phase[motor_id]][motor_id];
         //if (motor_speed[motor_id] >= 1)
         //   loggerMT(1);
         //loggerMU(0, "move_motor: %u", motor_countdowntimer[motor_id]);
         //change_motor_phase(motor_id);
         // cmp_phase++;
         // if (cmp_phase > 3)
         //    cmp_phase = 0;
         // cpu_to_mx1508(motor_id);
      }
      else
      {
         motor_countdowntimer[motor_id] -= elapsed;
      }
      //if (motor_speed[motor_id] >= 1)
      //   loggerMT(0);
      //}
}

void move_motors()
{
   // move_motor(MOTOR_PAN);
   // move_motor(MOTOR_TILT);
   // move_motor(MOTOR_ROTATE);
      int low = 0, high = 1;

      if (cmp_phase == 0)
      {
         gpio_write(13, high);
         gpio_write(165, low);
         gpio_write(12, low);
         gpio_write(183, low);
      }

      if (cmp_phase == 1)
      {
         gpio_write(13, low);
         gpio_write(165, low);
         gpio_write(12, high);
         gpio_write(183, low);
      }

      if (cmp_phase == 2)
      {
         gpio_write(13, low);
         gpio_write(165, high);
         gpio_write(12, low);
         gpio_write(183, low);
      }

      if (cmp_phase == 3)
      {
         gpio_write(13, low);
         gpio_write(165, low);
         gpio_write(12, low);
         gpio_write(183, high);
      }

      cmp_phase++;
      if (cmp_phase >= 4)
         cmp_phase = 0;
}

int process_message(char *msg, char * reply)
{
   int processed = 0;

   #define SPEED_SIGN 6
   #define SPEED_10X 7
   #define SPEED_1X 8
   #define SPEED_MOTORID 5

   loggerS("mtrctl: message [%s]", msg);

   if (!processed && msg[0] == 'p' && msg[1] == 'o' && msg[2] == 'w' && msg[6] == 'n')
   {
      //poweron
      logger("power on message received");
      processed = 1;
   }
   else if (!processed && msg[0] == 'p' && msg[1] == 'o' && msg[2] == 'w' && msg[6] == 'f')
   {
      //poweroff
      logger("power off message received");
      processed = 1;
   }
   else if (!processed && msg[0] == 's' && msg[4] == 'd')
   {
      //speed command format "speed[X|Y|Z][+|-]NN"

      int sign = msg[SPEED_SIGN];
      int x10 = msg[SPEED_10X] - '0';
      int x1 = msg[SPEED_1X] - '0';
      int speed = (10 * x10) + x1;
      //todo: fix this don't think the delay is needed anymore..hopefully.
      if (sign == '-')
      {
         speed *= -1;
         mcu_delay(1);
      }
      char mtrid[] = " ";
      mtrid[0] = msg[SPEED_MOTORID];
      loggerSI("mtrctl: motor speed change [%s][%d]", mtrid, speed);
      if (msg[SPEED_MOTORID] == 'X')
         motor_speed[MOTOR_PAN] = speed;
      else if (msg[SPEED_MOTORID] == 'Y')
         motor_speed[MOTOR_TILT] = speed;
      else if (msg[SPEED_MOTORID] == 'Z')
         motor_speed[MOTOR_ROTATE] = speed;
      logger("mtrctrl: motor speed changed");
      processed = 1;
   }

   return processed;
}

void main_loop()
{
   int len;
   char msg[64];
   char reply[64];

   gpio_setup(13, 1);
   gpio_setup(165, 1);
   gpio_setup(12, 1);
   gpio_setup(183, 1);

   gpio_setup(44, 1);
   gpio_setup(46, 1);
   gpio_setup(48, 1);
   gpio_setup(14, 1);

   gpio_setup(134, 1);
   gpio_setup(45, 1);
   gpio_setup(47, 1);
   gpio_setup(49, 1);
   logger("mtrctrl: initialized gpio");
   int run_slice_controller = 1;
   while (run_slice_controller)
   {
      // len = host_receive((unsigned char *)msg, 64);
      // while (len > 0)
      // {
      //    if (msg[len] == '\n')
      //    {
      //       msg[len] = '\0';
      //       reply[0] = '\0';
      //       process_message(msg, reply);
      //       if (reply[0] != '\0')
      //          host_send((unsigned char *)reply, 64);
      //       break;
      //    }
      //    len--;
      // }
      move_motors();
      mcu_delay(CLOCK_DELAY*2000);
   }
}

void mcu_main()
{
   while (1)
      mcu_sleep(1000);
   //logger("mtrctrl: mcu motor controller is up");
   //init_cpu_to_mx1508();
   motor_countdowntimer[0] = 1;
   motor_countdowntimer[1] = 1;
   motor_countdowntimer[2] = 1;
   logger("mtrctrl: initialized mx1508");
   main_loop();
}


int main_sliced_msg(char * msg, char * reply)
{
   int processed = 0;
   //debug_print(DBG_INFO, "main sliced msg\n");
   return processed;
}