#include "unistd.h"
#include <stdlib.h>

#include "mcu_api.h"

#include "wire.h"
#include "steppermotor.h"

#define StepperMotor_LOW  0
#define StepperMotor_HIGH 1
#define StepperMotor_POWER_OFF 0
#define StepperMotor_POWER_DOWN -1
#define StepperMotor_POWER_ON 1
#define StepperMotor_NUM_WIRE_STATES 8

unsigned char WIRESTATEMAP_HALFSTEP[StepperMotor_NUM_WIRE_STATES][StepperMotor_WIRE_COUNT] =
{
   //1-2 phase stepping
   // a+     a-     b+     b-
   { true , false, false, true },
   { true , false, false, false },
   { true , true , true , false },
   { false, true , true , false },
   { false, true , true , false },
   { false, true , true , true },
   { false, false, false, true },
   { true , false, false, true },
};
unsigned char WIRESTATEMAP_ONESTEP[StepperMotor_NUM_WIRE_STATES][StepperMotor_WIRE_COUNT] =
{
   //single phase stepping
   // a+     a-     b+     b-
   { true,  false, false, false },
   { false, false, true,  false },
   { false, true,  false, false },
   { false, false, false, true },
   { true,  false, false, false },
   { false, false, true,  false },
   { false, true,  false, false },
   { false, false, false, true },
};
unsigned char WIRESTATEMAP_TWOPHASE[StepperMotor_NUM_WIRE_STATES][StepperMotor_WIRE_COUNT] =
{
   //dual phase stepping
   // a+     a-     b+     b-
   { true, false, false, true },
   { true, false, true, false },
   { false, true, true, false },
   { false, true, false, true },
   { true, false, false, true },
   { true, false, true, false },
   { false, true, true, false },
   { false, true, false, true },
};
unsigned char WIRESTATEMAP_OFF[StepperMotor_WIRE_COUNT] =
{
   false, false, false, false,
};

static const int slowest = 20 + 12;
static const int fastest = 0;
static const int rate = 1;

void print_ttymcu1_oneintarg(char *format, int p1, int p2)
{
   char b[128];
   mcu_snprintf(b, 128, format, p1, p2);
   debug_print(DBG_INFO, b);
}

void print_ttymcu1_twointparg(char *format, const unsigned char * p1, int p2)
{
   char b[128];
   mcu_snprintf(b, 128, format, p1, p2);
   debug_print(DBG_INFO, b);
}

void print_ttymcu1_twointarg(char * format, int p1, int p2)
{
   char b[128];
   mcu_snprintf(b, 128, format, p1, p2);
   debug_print(DBG_INFO, b);
}

void print_ttymcu1_threeintarg(char *format, int p1, int p2, int p3)
{
   char b[128];
   mcu_snprintf(b, 128, format, p1, p2, p3);
   debug_print(DBG_INFO, b);
}

void StepperMotor_StepperMotor(StepperMotor* motor, int pinApositive, int pinAnegaitve, int pinBpositive, int pinBnegaitve)
{
   StepperMotor_initWires(motor, pinApositive, pinAnegaitve, pinBpositive, pinBnegaitve);
   motor->power = StepperMotor_POWER_OFF;
   motor->mode = MOTORMODE_STEP;
   motor->wireStateMap = WIRESTATEMAP_ONESTEP;
   motor->wireState = 0;
   motor->direction = StepperMotor_DIR_NONE;
   motor->motorState = StepperMotor_MOTORSTATE_STOPPED;
   motor->hold = false;
}

void StepperMotor_initWires(StepperMotor* motor, int pinApositive, int pinAnegaitve, int pinBpositive, int pinBnegaitve)
{
   Wire_connect(&motor->wires[StepperMotor_WIRE_A_POSITIVE], pinApositive, WIRE_DIRECTION_OUT);
   Wire_connect(&motor->wires[StepperMotor_WIRE_A_NEGATIVE], pinAnegaitve, WIRE_DIRECTION_OUT);
   Wire_connect(&motor->wires[StepperMotor_WIRE_B_POSITIVE], pinBpositive, WIRE_DIRECTION_OUT);
   Wire_connect(&motor->wires[StepperMotor_WIRE_B_NEGATIVE], pinBnegaitve, WIRE_DIRECTION_OUT);
}

void StepperMotor_initMotor(StepperMotor* motor)
{
    debug_print(DBG_INFO, "stepper motor init motor\n");
   //init motor winding A
   Wire_write(&motor->wires[StepperMotor_WIRE_A_POSITIVE], StepperMotor_LOW);
   Wire_write(&motor->wires[StepperMotor_WIRE_A_NEGATIVE], StepperMotor_LOW);

   Wire_write(&motor->wires[StepperMotor_WIRE_B_POSITIVE], StepperMotor_LOW);
   Wire_write(&motor->wires[StepperMotor_WIRE_B_NEGATIVE], StepperMotor_LOW);
}

void StepperMotor_sendState(StepperMotor* motor, unsigned char * wiresStates)
{
   if (WIRESTATEMAP_ONESTEP[motor->wireState][0])
      Wire_write(&motor->wires[StepperMotor_WIRE_A_POSITIVE], StepperMotor_HIGH);
   else
      Wire_write(&motor->wires[StepperMotor_WIRE_A_POSITIVE], StepperMotor_LOW);
   if (WIRESTATEMAP_ONESTEP[motor->wireState][1])
      Wire_write(&motor->wires[StepperMotor_WIRE_A_NEGATIVE], StepperMotor_HIGH);
   else
      Wire_write(&motor->wires[StepperMotor_WIRE_A_NEGATIVE], StepperMotor_LOW);
   if (WIRESTATEMAP_ONESTEP[motor->wireState][2])
      Wire_write(&motor->wires[StepperMotor_WIRE_B_POSITIVE], StepperMotor_HIGH);
   else
      Wire_write(&motor->wires[StepperMotor_WIRE_B_POSITIVE], StepperMotor_LOW);
   if (WIRESTATEMAP_ONESTEP[motor->wireState][3])
      Wire_write(&motor->wires[StepperMotor_WIRE_A_NEGATIVE], StepperMotor_HIGH);
   else
      Wire_write(&motor->wires[StepperMotor_WIRE_A_NEGATIVE], StepperMotor_LOW);

}

void StepperMotor_incWireState(StepperMotor* motor)
{
   motor->wireState += motor->direction;
   if (motor->wireState < 0)
      motor->wireState = StepperMotor_NUM_WIRE_STATES - 1;
   else if (motor->wireState >= StepperMotor_NUM_WIRE_STATES)
      motor->wireState = 0;
}

void StepperMotor_motion_mode(StepperMotor * motor, int mode)
{
   motor->mode = mode;
}

unsigned char StepperMotor_powerOn(StepperMotor* motor)
{
   unsigned char didPowerOn = false;
   if (motor->power != StepperMotor_POWER_ON)
   {
      debug_print(DBG_INFO, "stepper motor: power on\n");
      StepperMotor_initMotor(motor);
      motor->power = StepperMotor_POWER_ON;
      motor->direction = StepperMotor_DIR_NONE;
      motor->motorState = StepperMotor_MOTORSTATE_STOPPED;
      motor->speed = 0;
      motor->speed_delay_per_step = slowest;
      motor->wireState = 0;
      motor->acceleration = 0;
      motor->hold = false;
      didPowerOn = true;
   }
   return didPowerOn;
}

unsigned char StepperMotor_powerOff(StepperMotor* motor)
{
   unsigned char didPowerOff = false;
   if (motor->power != StepperMotor_POWER_OFF)
   {
      debug_print(DBG_INFO, "stepper motor: power off\n");
      StepperMotor_sendState(motor, WIRESTATEMAP_OFF);
      motor->direction = StepperMotor_DIR_NONE;
      motor->speed = 0;
      motor->motorState = StepperMotor_MOTORSTATE_STOPPED;
      motor->speed_delay_per_step = slowest;
      motor->wireState = 0;
      motor->acceleration = 0;
      motor->hold = false;
      motor->power = StepperMotor_POWER_OFF;
      didPowerOff = true;
   }
   return didPowerOff;
}

unsigned char StepperMotor_faster(StepperMotor* motor)
{
   unsigned char changed = false;

   debug_print(DBG_INFO, "steppermotor faster\n");

   if (StepperMotor_fasterOk(motor))
   {
      motor->acceleration -= rate;
      if (motor->acceleration < 0)
         motor->motorState = StepperMotor_MOTORSTATE_ACCEL;
      else if (motor->acceleration == 0)
         motor->motorState = StepperMotor_MOTORSTATE_CRUISE;
      changed = true;
   }

   debug_print(DBG_INFO, "steppermotor faster: done\n");
   return changed;
}

unsigned char StepperMotor_slower(StepperMotor* motor)
{
   unsigned char changed = false;

   debug_print(DBG_INFO, "steppermotor slower\n");

   if (StepperMotor_slowerOk(motor))
   {
      motor->motorState = StepperMotor_MOTORSTATE_DECEL;
      motor->acceleration += rate;
      if (motor->acceleration > 0)
         motor->motorState = StepperMotor_MOTORSTATE_DECEL;
      else if (motor->acceleration == 0)
         motor->motorState = StepperMotor_MOTORSTATE_CRUISE;
      changed = true;
      //char b[128];
      //mcu_snprintf(b, 128, "steppermotor: slower [%d][%d]\n", motor->speed_delay_per_step, motor->acceleration);
      //debug_print(DBG_INFO, b);
   }

   debug_print(DBG_INFO, "steppermotor slower: done\n");

   return changed;
}

unsigned char StepperMotor_fasterOk(StepperMotor* motor)
{
   print_ttymcu1_threeintarg("steppermotor: fasterok [%d][%d][%d]\n", motor->motorState, motor->speed_delay_per_step, motor->acceleration);
   if (motor->speed_delay_per_step + motor->acceleration > fastest)
   {
      debug_print(DBG_INFO, "steppermotor: fasterok : true\n");
      return true;
   }
   else
   {
      debug_print(DBG_INFO, "steppermotor: fasterok : false\n");
      return false;
   }
}

unsigned char StepperMotor_slowerOk(StepperMotor* motor)
{
   //char b[128];
   //mcu_snprintf(b, 128, "steppermotor: slowerok [%d][%d]\n", motor->speed_delay_per_step, motor->acceleration);
   //debug_print(DBG_INFO, b);
   if (motor->motorState != StepperMotor_MOTORSTATE_STOPPED && motor->speed_delay_per_step + motor->acceleration < slowest)
   {
      debug_print(DBG_INFO, "steppermotor: slowerok : true\n");
      return true;
   }
   else
   {
      debug_print(DBG_INFO, "steppermotor: slowerok : false\n");
      return false;
   }
}

void StepperMotor_step(StepperMotor *motor, int _direction)
{
   if (motor->motorState == StepperMotor_MOTORSTATE_STOPPED &&
       motor->hold == false)
   {
      StepperMotor_sendState(motor, motor->wireStateMap[motor->wireState]);
      mcu_delay(1000);
      motor->direction = _direction;
      StepperMotor_incWireState(motor);
      StepperMotor_sendState(motor, motor->wireStateMap[motor->wireState]);
      mcu_delay(1000);
      StepperMotor_sendState(motor, WIRESTATEMAP_OFF);
      motor->direction = StepperMotor_DIR_NONE;
      debug_print(DBG_INFO, "DID: step \n");
   }
   else
   {
      debug_print(DBG_INFO, "DID NOT: step \n");
   }
}

unsigned char StepperMotor_autostep_running(StepperMotor* motor)
{
   return motor->motorState != StepperMotor_MOTORSTATE_STOPPED;
}

void StepperMotor_autostep_run(StepperMotor* motor, int _direction)
{
   debug_print(DBG_INFO, "autostep run\n");
   if (motor->mode != MOTORMODE_AUTOSTEP)
      motor->mode = MOTORMODE_AUTOSTEP;

   motor->speed = 0;
   motor->speed_delay_per_step = slowest;
   motor->direction = _direction;
   motor->motorState = StepperMotor_MOTORSTATE_STOPPED;
   motor->wireState = 0;
   motor->hold = true;
   debug_print(DBG_INFO, "autostep runnning\n");
}

void StepperMotor_autostep_stop(StepperMotor* motor, unsigned char hold)
{
   debug_print(DBG_INFO, "autostep stop\n");
   if (motor->mode != MOTORMODE_AUTOSTEP)
      motor->mode = MOTORMODE_AUTOSTEP;
   if (motor->motorState != StepperMotor_MOTORSTATE_DECEL_STOP &&
      motor->motorState != StepperMotor_MOTORSTATE_STOPPED)
   {
      motor->hold = hold;
      motor->motorState = StepperMotor_MOTORSTATE_DECEL_STOP;
      motor->acceleration = slowest - motor->speed_delay_per_step;
   }
   else if (motor->motorState == StepperMotor_MOTORSTATE_STOPPED && hold == false && motor->hold == true)
   {
      StepperMotor_sendState(motor, WIRESTATEMAP_OFF);
      motor->hold = false;
   }
   debug_print(DBG_INFO, "autostep stopped\n");
}

void StepperMotor_autostep_speed(StepperMotor *motor, int speed)
{
   print_ttymcu1_threeintarg("autostep speed [%d][%d][%d]\n", motor->mode, motor->speed, speed);
   if (motor->mode != MOTORMODE_AUTOSTEP)
      motor->mode = MOTORMODE_AUTOSTEP;

   if (speed == 0 && motor->speed != 0)
   {
      StepperMotor_autostep_stop(motor, true);
      motor->speed = speed;
      debug_print(DBG_INFO, "steppermotor: stop\n");
   }
   else if (speed > motor->speed)
   {
      if (!StepperMotor_autostep_running(motor) || motor->direction == StepperMotor_DIR_CCW)
         StepperMotor_autostep_run(motor, StepperMotor_DIR_CW);
      if (speed > motor->speed)
          StepperMotor_faster(motor);
      motor->speed = speed;
   }
   else if (speed < motor->speed)
   {
      if (!StepperMotor_autostep_running(motor) || motor->direction == StepperMotor_DIR_CW)
         StepperMotor_autostep_run(motor, StepperMotor_DIR_CCW);
      if (speed < motor->speed)
         StepperMotor_slower(motor);
      motor->speed = speed;
   }
   debug_print(DBG_INFO, "autostep speed done\n");
}

void StepperMotor_toggleStepMode(StepperMotor* motor)
{
   if (motor->wireStateMap == WIRESTATEMAP_HALFSTEP)
      motor->wireStateMap = WIRESTATEMAP_ONESTEP;
   else if (motor->wireStateMap == WIRESTATEMAP_ONESTEP)
      motor->wireStateMap = WIRESTATEMAP_TWOPHASE;
   else if (motor->wireStateMap == WIRESTATEMAP_TWOPHASE)
      motor->wireStateMap = WIRESTATEMAP_HALFSTEP;
}

void StepperMotor_UpdateSpeed(StepperMotor* motor)
{
   if (motor->motorState == StepperMotor_MOTORSTATE_ACCEL)
   {
      print_ttymcu1_twointarg("steppermotor: accel [%d][%d]\n", motor->speed_delay_per_step, motor->acceleration);
      motor->speed_delay_per_step -= rate;
      motor->acceleration += rate;
      print_ttymcu1_twointarg("steppermotor: accel2 [%d][%d]\n", motor->speed_delay_per_step, motor->acceleration);
      if (motor->acceleration == 0)
      {
         motor->motorState = StepperMotor_MOTORSTATE_CRUISE;
         debug_print(DBG_INFO, "steppermotor:accel: accel=0 cruise\n");
      }
   }
   else if (motor->motorState == StepperMotor_MOTORSTATE_DECEL)
   {
      print_ttymcu1_twointarg("steppermotor: decel [%d][%d]\n", motor->speed_delay_per_step, motor->acceleration);
      motor->speed_delay_per_step += rate;
      motor->acceleration -= rate;
      print_ttymcu1_twointarg("steppermotor: decel2 [%d][%d]\n", motor->speed_delay_per_step, motor->acceleration);
      if (motor->acceleration == 0)
      {
         motor->motorState = StepperMotor_MOTORSTATE_CRUISE;
         debug_print(DBG_INFO, "steppermotor:decel3: cruise\n");
      }
   }
   else if (motor->motorState == StepperMotor_MOTORSTATE_DECEL_STOP)
   {
      print_ttymcu1_twointarg("steppermotor: decel_stop [%d][%d]\n", motor->speed_delay_per_step, motor->acceleration);
      if (motor->acceleration)
      {
         motor->speed_delay_per_step += rate;
         motor->acceleration -= rate;
      }
      if (motor->acceleration == 0)
      {
         debug_print(DBG_INFO, "steppermotor:decel_stop: accel=0 stopped\n");
         motor->motorState = StepperMotor_MOTORSTATE_STOPPED;
         motor->direction = StepperMotor_DIR_NONE;
         if (!motor->hold)
         {
            StepperMotor_sendState(motor, WIRESTATEMAP_OFF);
         }
      }
   }
}

int StepperMotor_loop (StepperMotor* motor)
{
   static unsigned long last_time;

   if (motor->power == StepperMotor_POWER_ON && ((motor->motorState != StepperMotor_MOTORSTATE_STOPPED) || (motor->hold == true)))
   {
      if (motor->direction != StepperMotor_DIR_NONE)
      {
         if (last_time == 0 || time_ms() - last_time > motor->speed_delay_per_step)
         {
            StepperMotor_sendState(motor, motor->wireStateMap[motor->wireState]);
            StepperMotor_incWireState(motor);
            StepperMotor_UpdateSpeed(motor);
            last_time = time_ms();
         }
      }
      else
      {
         if (motor->hold)
         {
            StepperMotor_sendState(motor, motor->wireStateMap[motor->wireState]);
         }
      }
   }
   return 0;
}

