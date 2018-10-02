
#include "stdlib.h"
#include "string.h"
#include "wire.h"
#include "time.h"
#include "pthread.h"

#define NUM_MOTORS   3
#define STEPS_PER_REVOLUTION 200
#define MIN_PHASE    0
#define MAX_PHASE    3
#define NUM_PHASE    4
#define PHASE_OFF    -1
#define CLOCK_DELAY  50

#define POWER_OFF          0
#define POWER_ON_WAIT      1
#define POWER_ON           2

#define NUM_POWER_LEVELS      15
#define MOTOR_PAN             0
#define MOTOR_TILT            1
#define MOTOR_ROTATE          2
#define scale                 200
#define MOTOR_DELAY_IDX_0_POWER (NUM_POWER_LEVELS/2)

#define SPEED_SIGN      6
#define SPEED_10X       7
#define SPEED_1X        8
#define SPEED_MOTORID   5

//command format to step the motor.
//command format: "STEP", axis, speed, steps
//STEP[XYZ][+-][0-9]{2}[0-9]{4}
#define STEPCMD_MOTORID       4
#define STEPCMD_SPEED_SIGN    5
#define STEPCMD_SPEED_10X     6
#define STEPCMD_SPEED_1X      7
#define STEPCMD_STEPS_1000X   8
#define STEPCMD_STEPS_100X    9
#define STEPCMD_STEPS_10X     10
#define STEPCMD_STEPS_1X      11


#define GPIO_MTRPAN_IN1     13
#define GPIO_MTRPAN_IN2     165
#define GPIO_MTRPAN_IN3     12
#define GPIO_MTRPAN_IN4     183

#define GPIO_MTRTILTU_IN1   44
#define GPIO_MTRTILTU_IN2   46
#define GPIO_MTRTILTU_IN3   48
#define GPIO_MTRTILTU_IN4   14

#define GPIO_MTRROTATE_IN1  182
#define GPIO_MTRROTATE_IN2  114
#define GPIO_MTRROTATE_IN3  129
#define GPIO_MTRROTATE_IN4  131

#define IDLE_PWM_RATE         3UL
#define POWER_ON_INTERVAL_MS  500


extern void slice_controller_run();
extern void run_sliced_msg_distribute(char *msg, int msg_len);

int motor_speed[NUM_MOTORS] = {0, 0, 0};
int motor_phase[NUM_MOTORS] = {0, 0, 0};
int motor_speed_delay[NUM_POWER_LEVELS][NUM_MOTORS] =
{
   {  8 * scale,  8 * scale,  8 * scale},  //0
   { 16 * scale, 16 * scale, 16 * scale},  //1
   { 30 * scale, 30 * scale, 30 * scale},  //2
   { 60 * scale, 60 * scale, 60 * scale},  //3
   { 90 * scale, 90 * scale, 90 * scale},  //4
   {120 * scale,120 * scale,120 * scale},  //5
   {180 * scale,180 * scale,180 * scale},  //6
   {  1 * scale,  1 * scale,  1 * scale},  //7
   {180 * scale,180 * scale,180 * scale},
   {120 * scale,120 * scale,120 * scale},
   { 90 * scale, 90 * scale, 90 * scale},
   { 60 * scale, 60 * scale, 60 * scale},
   { 30 * scale, 30 * scale, 30 * scale},
   { 16 * scale, 16 * scale, 16 * scale},
   {  8 * scale,  8 * scale,  8 * scale},
};
unsigned long motor_countdowntimer[NUM_MOTORS] = {2000, 2000, 2000};
unsigned long motor_idle_pwm_rate[NUM_MOTORS] = {IDLE_PWM_RATE, IDLE_PWM_RATE, IDLE_PWM_RATE};
unsigned long motor_idle_pwm[NUM_MOTORS] = {0, 0, 0};
unsigned long motor_countdowntimes_lasttime[NUM_MOTORS] = {0, 0, 0};
int motor_step_countdown[NUM_MOTORS] = {0, 0, 0};
int motor_steps_per_revolution[NUM_MOTORS] = {200, 200, 200};
int motor_steps_from_home[NUM_MOTORS] = {0, 0, 0};
struct Wire gpio_lines[12];
int power_state[NUM_MOTORS] = {POWER_OFF, POWER_OFF, POWER_OFF};

unsigned long power_up_time_ms = 0;

unsigned long time_micros()
{
   struct timespec now;

   clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);

   return now.tv_sec * 1e6 + now.tv_nsec / 1e3;
}

unsigned long time_millis()
{
   return time_micros() / 1000;
}

void init_cpu_to_mx1508()
{
   printf("Initializing cpu_to_mx1508 Gpio\n");
   gpio_lines[0].connectDigital(GPIO_MTRPAN_IN1, 0, 0);
   gpio_lines[1].connectDigital(GPIO_MTRPAN_IN2, 0, 0);
   gpio_lines[2].connectDigital(GPIO_MTRPAN_IN3, 0, 0);
   gpio_lines[3].connectDigital(GPIO_MTRPAN_IN4, 0, 0);

   gpio_lines[4].connectDigital(GPIO_MTRTILTU_IN1, 0, 0);
   gpio_lines[5].connectDigital(GPIO_MTRTILTU_IN2, 0, 0);
   gpio_lines[6].connectDigital(GPIO_MTRTILTU_IN3, 0, 0);
   gpio_lines[7].connectDigital(GPIO_MTRTILTU_IN4, 0, 0);

   gpio_lines[8].connectDigital(GPIO_MTRROTATE_IN1, 0, 0);
   gpio_lines[9].connectDigital(GPIO_MTRROTATE_IN2, 0, 0);
   gpio_lines[10].connectDigital(GPIO_MTRROTATE_IN3, 0, 0);
   gpio_lines[11].connectDigital(GPIO_MTRROTATE_IN4, 0, 0);
   motor_countdowntimes_lasttime[MOTOR_PAN] = time_micros();
   //motor_countdowntimer[MOTOR_PAN] = motor_speed_delay[MOTOR_POWER_0][MOTOR_PAN];
   motor_countdowntimes_lasttime[MOTOR_TILT] = time_micros();
   //motor_countdowntimer[MOTOR_TILT] = motor_speed_delay[MOTOR_POWER_0][MOTOR_TILT];
   motor_countdowntimes_lasttime[MOTOR_ROTATE] = time_micros();
   //motor_countdowntimer[MOTOR_ROTATE] = motor_speed_delay[MOTOR_POWER_0][MOTOR_ROTATE];
}

void mx1508_to_motor(int phase, int a, int b, int c, int d)
{
   int low = 0, high = 1;

   if (phase == 0)
   {
      gpio_lines[a].write(high);
      gpio_lines[b].write(low);
      gpio_lines[c].write(low);
      gpio_lines[d].write(low);

   }

   if (phase == 1)
   {
      gpio_lines[a].write(low);
      gpio_lines[b].write(low);
      gpio_lines[c].write(high);
      gpio_lines[d].write(low);
   }

   if (phase == 2)
   {
      gpio_lines[a].write(low);
      gpio_lines[b].write(high);
      gpio_lines[c].write(low);
      gpio_lines[d].write(low);
   }

   if (phase == 3)
   {
      gpio_lines[a].write(low);
      gpio_lines[b].write(low);
      gpio_lines[c].write(low);
      gpio_lines[d].write(high);
   }

   if (phase == PHASE_OFF)
   {
      gpio_lines[a].write(low);
      gpio_lines[b].write(low);
      gpio_lines[c].write(low);
      gpio_lines[d].write(low);
   }
}

void cpu_to_mx1508(int motor_id)
{
   int in1, in2, in3, in4;

   if (motor_id == MOTOR_PAN)
   {
      in1 = 0;
      in2 = 1;
      in3 = 2;
      in4 = 3;
   }
   if (motor_id == MOTOR_TILT)
   {
      in1 = 4;
      in2 = 5;
      in3 = 6;
      in4 = 7;
   }
   if (motor_id == MOTOR_ROTATE)
   {
      in1 = 8;
      in2 = 9;
      in3 = 10;
      in4 = 11;
   }

   if (power_state[motor_id] == POWER_ON)
      mx1508_to_motor(motor_phase[motor_id], in1, in2, in3, in4);
   else
      mx1508_to_motor(PHASE_OFF, in1, in2, in3, in4);
}

void change_motor_phase(int motor_id)
{
   int restore_phase = -100;

   if (motor_speed[motor_id] > 0)
   {
      motor_phase[motor_id]++;
      if (motor_phase[motor_id] >= NUM_PHASE)
         motor_phase[motor_id] = 0;
   }
   else if (motor_speed[motor_id] < 0)
   {
      motor_phase[motor_id]--;
      if (motor_phase[motor_id] < 0)
         motor_phase[motor_id] = NUM_PHASE - 1;
   }
   else
   {
      if (motor_idle_pwm[motor_id] % motor_idle_pwm_rate[motor_id] == 0)
      {
         restore_phase = motor_phase[motor_id];
         motor_phase[motor_id] = PHASE_OFF;
      }
      motor_idle_pwm[motor_id]++;
   }

   cpu_to_mx1508(motor_id);

   if (restore_phase != -100)
      motor_phase[motor_id] = restore_phase;
}

void move_motor(int motor_id)
{
   unsigned long time_now = time_micros();
   unsigned long elapsed = time_now - motor_countdowntimes_lasttime[motor_id];
   motor_countdowntimes_lasttime[motor_id] = time_now;

   if (motor_countdowntimer[motor_id] <= elapsed)
   {
      motor_countdowntimer[motor_id] = motor_speed_delay[motor_speed[motor_id] + MOTOR_DELAY_IDX_0_POWER][motor_id];
      //printf("timeout: %lu %d\n", motor_countdowntimer[motor_id], motor_speed[motor_id] + MOTOR_DELAY_IDX_0_POWER);
      change_motor_phase(motor_id);
      if (motor_speed[motor_id])
      {
         motor_steps_from_home[motor_id] += motor_speed[motor_id] > 0 ? 1 : -1;
         if (motor_step_countdown[motor_id])
         {
            motor_step_countdown[motor_id]--;
            if (motor_step_countdown[motor_id] <= 0)
            {
               printf("motor stepping complete [%+3d] [%+3d] [%+3d]\n", motor_steps_from_home[0], motor_steps_from_home[1], motor_steps_from_home[2]);
               motor_speed[motor_id] = 0;
            }
         }
      }
   }
   else
   {
      motor_countdowntimer[motor_id] -= elapsed;
   }
}

void move_motors()
{
   move_motor(MOTOR_PAN);
   move_motor(MOTOR_TILT);
   move_motor(MOTOR_ROTATE);
}

int mcu_process_message(char *msg, char *reply)
{
   int processed = 0;

   if (!processed && msg[0] == 'p' && msg[1] == 'o' && msg[2] == 'w' && msg[6] == 'n')
   {
      //poweron
      power_up_time_ms = time_millis();
      power_state[MOTOR_TILT] = POWER_ON_WAIT;
      power_state[MOTOR_PAN] = POWER_ON_WAIT;

      power_state[MOTOR_ROTATE] = POWER_ON;
      usleep(POWER_ON_INTERVAL_MS * 200);
      power_state[MOTOR_TILT] = POWER_ON;
      usleep(POWER_ON_INTERVAL_MS * 200);
      power_state[MOTOR_PAN] = POWER_ON;
      usleep(POWER_ON_INTERVAL_MS * 200);

      printf("motors powered on\n");

      processed = 1;
   }
   else if (!processed && msg[0] == 'p' && msg[1] == 'o' && msg[2] == 'w' && msg[6] == 'f')
   {
      //poweroff
      if (power_state[MOTOR_PAN] != POWER_OFF)
         power_state[MOTOR_PAN] = POWER_OFF;
      if (power_state[MOTOR_TILT] != POWER_OFF)
         power_state[MOTOR_TILT] = POWER_OFF;
      if (power_state[MOTOR_ROTATE] != POWER_OFF)
         power_state[MOTOR_ROTATE] = POWER_OFF;
      printf("powered off motors\n");
      processed = 1;
   }
   else if (!processed && msg[0] == 's' && msg[4] == 'd')
   {
      //speed command format "speed[X|Y|Z][+|-]NN"

      int sign = msg[SPEED_SIGN];
      int x10 = msg[SPEED_10X] - '0';
      int x1 = msg[SPEED_1X] - '0';
      int speed = (10 * x10) + x1;

      if (sign == '-')
      {
         speed *= -1;
      }

      printf("motor speed now = %d -> %s", speed, msg);
      if (speed == 0)
      {
         if (msg[SPEED_MOTORID] == 'X')
            motor_step_countdown[MOTOR_PAN] = 1;
         else if (msg[SPEED_MOTORID] == 'Y')
            motor_step_countdown[MOTOR_TILT] = 1;
         else if (msg[SPEED_MOTORID] == 'Z')
            motor_step_countdown[MOTOR_ROTATE] = 1;
      }
      else
      {
         if (msg[SPEED_MOTORID] == 'X')
            motor_speed[MOTOR_PAN] = speed;
         else if (msg[SPEED_MOTORID] == 'Y')
            motor_speed[MOTOR_TILT] = speed;
         else if (msg[SPEED_MOTORID] == 'Z')
            motor_speed[MOTOR_ROTATE] = speed;
      }

      processed = 1;
   }
   else if (!processed && msg[0] == 's' && msg[3] == 'p')
   {
      //step command format "step[X|Y|Z][+|-]NNSSSS"
      //NN = speed
      //SSSS = steps

      int sign = msg[STEPCMD_SPEED_SIGN];
      int speedx10 = msg[STEPCMD_SPEED_10X] - '0';
      int speedx1 = msg[STEPCMD_SPEED_1X] - '0';
      int stepsx1000 = msg[STEPCMD_STEPS_1000X] - '0';
      int stepsx100 = msg[STEPCMD_STEPS_100X] - '0';
      int stepsx10 = msg[STEPCMD_STEPS_10X] - '0';
      int stepsx1 = msg[STEPCMD_STEPS_1X] - '0';

      int speed = (10 * speedx10) + speedx1;
      int steps = (1000 * stepsx1000) + (100 * stepsx100)+ (10 * stepsx10) + stepsx1;

      if (sign == '-')
      {
         speed *= -1;
      }

      printf("stepping motor id[%c] speed[%+02d] steps[%04d]\n", msg[STEPCMD_MOTORID], speed, steps);
      if (msg[STEPCMD_MOTORID] == 'X')
      {
         motor_step_countdown[MOTOR_PAN] = steps;
         motor_speed[MOTOR_PAN] = speed;
      }
      else if (msg[STEPCMD_MOTORID] == 'Y')
      {
         motor_step_countdown[MOTOR_TILT] = steps;
         motor_speed[MOTOR_TILT] = speed;
      }
      else if (msg[STEPCMD_MOTORID] == 'Z')
      {
         motor_step_countdown[MOTOR_ROTATE] = steps;
         motor_speed[MOTOR_ROTATE] = speed;
      }

      processed = 1;
   }
   else if (!processed && msg[0] == 'p' && msg[1] == 'o' && msg[2] == 's')
   {
      sprintf(reply, "%+03d %+03d %+03d\n", motor_steps_from_home[0], motor_steps_from_home[1], motor_steps_from_home[2]);

      processed = 1;
   }
   return processed;
}

void* main_loop(void * arg);
pthread_t main_loop_tid;

void mcu_main_init()
{
   motor_countdowntimer[0] = 0;
   motor_countdowntimer[1] = 0;
   motor_countdowntimer[2] = 0;

   init_cpu_to_mx1508();

   pthread_create(&main_loop_tid, NULL, &main_loop, NULL);
}

void* main_loop(void * arg)
{
   while(1)
   {
      move_motors();
      //mcu_delay(CLOCK_DELAY * 2000);
      usleep(CLOCK_DELAY);
      //printf("mcu_motor_running\n");
   }
}
