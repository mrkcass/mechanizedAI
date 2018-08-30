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

const bool WIRESTATEMAP_HALFSTEP[StepperMotor_NUM_WIRE_STATES][StepperMotor_WIRE_COUNT] =
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
const bool WIRESTATEMAP_ONESTEP[StepperMotor_NUM_WIRE_STATES][StepperMotor_WIRE_COUNT] =
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
const bool WIRESTATEMAP_TWOPHASE[StepperMotor_NUM_WIRE_STATES][StepperMotor_WIRE_COUNT] =
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
const bool WIRESTATEMAP_OFF[StepperMotor_WIRE_COUNT] =
{
   false, false, false, false,
};

static const int slowest = 20 + 12;
static const int fastest = 0;
static const int rate = 1;

StepperMotor_StepperMotor(StepperMotor* motor, int pinApositive, int pinAnegaitve, int pinBpositive, int pinBnegaitve)
{
   StepperMotor_initWires(motor, pinApositive, pinAnegaitve, pinBpositive, pinBnegaitve);
   motor->power = StepperMotor_POWER_OFF;
   motor->mode = MOTORMODE_STEP;
   motor->wireStateMap = WIRESTATEMAP_ONESTEP;
   motor->wireState = 0;
   motor->direction = StepperMotor_DIR_NONE;
   motor->motorState = StepperMotor_MOTORSTATE_STOPPED;
   motor->hold = false;
   return motor;
}

void StepperMotor_initWires(StepperMotor* motor, int pinApositive, int pinAnegaitve, int pinBpositive, int pinBnegaitve)
{
   Wire_connect(&motor->wires[StepperMotor_WIRE_A_POSITVE], pinApositive, WIRE_DIRECTION_OUT);
   Wire_connect(&motor->wires[StepperMotor_WIRE_A_NEGATIVE], pinAnegaitve, WIRE_DIRECTION_OUT);
   Wire_connect(&motor->wires[StepperMotor_WIRE_B_POSITIVE], pinBpositive, WIRE_DIRECTION_OUT);
   Wire_connect(&motor->wires[StepperMotor_WIRE_B_NEGATIVE], pinBnegaitve, WIRE_DIRECTION_OUT);
}

void StepperMotor_initMotor(StepperMotor* motor)
{
   //init motor winding A
   Wire_write(&motor->wires[StepperMotor_WIRE_A_POSITVE], StepperMotor_LOW);
   Wire_write(&motor->wires[StepperMotor_WIRE_A_NEGATIVE], StepperMotor_LOW);

   Wire_write(&motor->wires[StepperMotor_WIRE_B_POSITIVE], StepperMotor_LOW);
   Wire_write(&motor->wires[StepperMotor_WIRE_B_NEGATIVE], StepperMotor_LOW);
}

void StepperMotor_sendState(StepperMotor* motor, const bool wiresStates[StepperMotor_WIRE_COUNT])
{
   Wire_write(&motor->wires[StepperMotor_WIRE_A_POSITVE], wiresStates[0] ? StepperMotor_HIGH :  StepperMotor_LOW);
   Wire_write(&motor->wires[StepperMotor_WIRE_A_NEGATIVE], wiresStates[1] ? StepperMotor_HIGH : StepperMotor_LOW);
   Wire_write(&motor->wires[StepperMotor_WIRE_B_POSITIVE], wiresStates[2] ? StepperMotor_HIGH : StepperMotor_LOW);
   Wire_write(&motor->wires[StepperMotor_WIRE_B_NEGATIVE], wiresStates[3] ? StepperMotor_HIGH : StepperMotor_LOW);
}

void StepperMotor_incWireState(StepperMotor* motor)
{
   motor->wireState += motor->direction;
   if (motor->wireState < 0)
      motor->wireState = StepperMotor_NUM_WIRE_STATES - 1;
   else if (motor->wireState >= StepperMotor_NUM_WIRE_STATES)
      motor->wireState = 0;
}

bool StepperMotor_powerOn(StepperMotor* motor)
{
   bool didPowerOn = false;
   if (motor->power != StepperMotor_POWER_ON)
   {
      StepperMotor_initMotor(motor);
      motor->power = StepperMotor_POWER_ON;
      motor->direction = StepperMotor_DIR_NONE;
      motor->motorState = StepperMotor_MOTORSTATE_STOPPED;
      motor->speed = slowest;
      motor->wireState = 0;
      motor->acceleration = 0;
      motor->hold = false;
      didPowerOn = true;
   }
   return didPowerOn;
}

bool StepperMotor_powerOff(StepperMotor* motor)
{
   bool didPowerOff = false;
   if (motor->power != StepperMotor_POWER_OFF)
   {
      StepperMotor_sendState(motor, WIRESTATEMAP_OFF);
      motor->direction = StepperMotor_DIR_NONE;
      motor->motorState = StepperMotor_MOTORSTATE_STOPPED;
      motor->speed = slowest;
      motor->wireState = 0;
      motor->acceleration = 0;
      motor->hold = false;
      motor->power = StepperMotor_POWER_OFF;
      didPowerOff = true;
   }
   return didPowerOff;
}

bool StepperMotor_faster(StepperMotor* motor)
{
   bool changed = false;

   if (StepperMotor_fasterOk(motor))
   {
      motor->acceleration -= rate;
      if (motor->acceleration < 0)
         motor->motorState = StepperMotor_MOTORSTATE_ACCEL;
      else if (motor->acceleration == 0)
         motor->motorState = StepperMotor_MOTORSTATE_CRUISE;
      changed = true;
      char b[128];
      mcu_snprintf(b, 128, "steppermotor: faster [%d][%d]\n", motor->speed, motor->acceleration);
      debug_print(DBG_INFO, b);
   }

   return changed;
}

bool StepperMotor_slower(StepperMotor* motor)
{
   bool changed = false;

   if (StepperMotor_slowerOk(motor))
   {
      motor->motorState = StepperMotor_MOTORSTATE_DECEL;
      motor->acceleration += rate;
      if (motor->acceleration > 0)
         motor->motorState = StepperMotor_MOTORSTATE_DECEL;
      else if (motor->acceleration == 0)
         motor->motorState = StepperMotor_MOTORSTATE_CRUISE;
      changed = true;
      char b[128];
      mcu_snprintf(b, 128, "steppermotor: slower [%d][%d]\n", motor->speed, motor->acceleration);
      debug_print(DBG_INFO, b);
   }

   return changed;
}

bool StepperMotor_fasterOk(StepperMotor* motor)
{
   char b[128];
   mcu_snprintf(b, 128, "steppermotor: fasterok [%d][%d]\n", motor->speed, motor->acceleration);
   debug_print(DBG_INFO, b);
   if (motor->motorState != StepperMotor_MOTORSTATE_STOPPED && motor->speed + motor->acceleration > fastest)
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

bool StepperMotor_slowerOk(StepperMotor* motor)
{
   char b[128];
   mcu_snprintf(b, 128, "steppermotor: slowerok [%d][%d]\n", motor->speed, motor->acceleration);
   debug_print(DBG_INFO, b);
   if (motor->motorState != StepperMotor_MOTORSTATE_STOPPED && motor->speed + motor->acceleration < slowest)
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

bool StepperMotor_running(StepperMotor* motor)
{
   return motor->motorState != StepperMotor_MOTORSTATE_STOPPED;
}

void StepperMotor_autostep_enable(StepperMotor* motor, int _direction)
{
   debug_print(DBG_INFO, "steppermotor: run\n");
   if (motor->mode == MOTORMODE_STEP)
   {
      debug_print(DBG_INFO, "steppermotor: auto step enabled\n");
      motor->mode = MOTORMODE_AUTOSTEP;
      motor->speed = slowest;
      motor->direction = _direction;
      motor->motorState = StepperMotor_MOTORSTATE_CRUISE;
      motor->wireState = 0;
   }
}

void StepperMotor_autostep_disable(StepperMotor* motor, bool hold)
{
   debug_print(DBG_INFO, "steppermotor: stop\n");
   if (motor->mode != MOTORMODE_AUTOSTEP)
      return;
   if (motor->motorState != StepperMotor_MOTORSTATE_DECEL_STOP &&
      motor->motorState != StepperMotor_MOTORSTATE_STOPPED)
   {
      motor->hold = hold;
      motor->motorState = StepperMotor_MOTORSTATE_DECEL_STOP;
      motor->acceleration = slowest - motor->speed;
      char b[128];
      mcu_snprintf(b, 128, "steppermotor: stop2 [%d][%d]\n", motor->speed, motor->acceleration);
      debug_print(DBG_INFO, b);
   }
   else if (motor->motorState == StepperMotor_MOTORSTATE_STOPPED && motor->hold == true)
   {
      StepperMotor_sendState(motor, WIRESTATEMAP_OFF);
      motor->hold = false;
   }
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
      char b[128];
      mcu_snprintf(b, 128, "steppermotor: accel [%d][%d]\n", motor->speed, motor->acceleration);
      debug_print(DBG_INFO, b);
      motor->speed -= rate;
      motor->acceleration += rate;
      mcu_snprintf(b, 128, "steppermotor: accel [%d][%d]\n", motor->speed, motor->acceleration);
      debug_print(DBG_INFO, b);
      if (motor->acceleration == 0)
      {
         motor->motorState = StepperMotor_MOTORSTATE_CRUISE;
         debug_print(DBG_INFO, "steppermotor:accel: accel=0 cruise\n");
      }
   }
   else if (motor->motorState == StepperMotor_MOTORSTATE_DECEL)
   {
      char b[128];
      mcu_snprintf(b, 128, "steppermotor: decel [%d][%d]\n", motor->speed, motor->acceleration);
      debug_print(DBG_INFO, b);
      motor->speed += rate;
      motor->acceleration -= rate;
      mcu_snprintf(b, 128, "steppermotor: decel2 [%d][%d]\n", motor->speed, motor->acceleration);
      debug_print(DBG_INFO, b);
      if (motor->acceleration == 0)
      {
         motor->motorState = StepperMotor_MOTORSTATE_CRUISE;
         debug_print(DBG_INFO, "steppermotor:decel3: cruise\n");
      }
   }
   else if (motor->motorState == StepperMotor_MOTORSTATE_DECEL_STOP)
   {
      char b[128];
      mcu_snprintf(b, 128, "steppermotor: decel_stop [%d][%d]\n", motor->speed, motor->acceleration);
      debug_print(DBG_INFO, b);
      if (motor->acceleration)
      {
         motor->speed += rate;
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
         if (last_time == 0 || time_ms() - last_time > motor->speed)
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

