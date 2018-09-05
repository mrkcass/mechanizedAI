#ifndef __steppermotor_h__
#define __steppermotor_h__

#include "wire.h"

#define false 0
#define true 1

enum
{
   StepperMotor_WIRE_A_POSITIVE = 0,
   StepperMotor_WIRE_A_NEGATIVE,
   StepperMotor_WIRE_B_POSITIVE,
   StepperMotor_WIRE_B_NEGATIVE,
};
#define StepperMotor_WIRE_COUNT 4
enum
{
   StepperMotor_MOTORSTATE_STOPPED = 0,
   StepperMotor_MOTORSTATE_ACCEL,
   StepperMotor_MOTORSTATE_CRUISE,
   StepperMotor_MOTORSTATE_DECEL,
   StepperMotor_MOTORSTATE_DECEL_STOP,
};

#define MOTORMODE_STEP 		0
#define MOTORMODE_AUTOSTEP 1
#define MOTORMODE_GOTO		2

#define StepperMotor_DIR_CCW  1
#define StepperMotor_DIR_NONE 0
#define StepperMotor_DIR_CW   -1

typedef unsigned char WIRE_ARRAY[StepperMotor_WIRE_COUNT];

typedef struct _StepperMotor_
{
   int power;
   int mode;
   int speed;
   int direction;
   unsigned char hold;

   int speed_delay_per_step;
   int acceleration;
   int wireState;
   int motorState;
   Wire wires[StepperMotor_WIRE_COUNT];
   WIRE_ARRAY * wireStateMap;
} StepperMotor;

void StepperMotor_StepperMotor(StepperMotor* motor, int pinApositive, int pinAnegaitve, int pinBpositive, int pinBnegaitve);
void StepperMotor_initWires(StepperMotor* motor, int pinApositive, int pinAnegaitve, int pinBpositive, int pinBnegaitve);
void StepperMotor_initMotor(StepperMotor* motor);
void StepperMotor_sendState(StepperMotor* motor, unsigned char * wiresStates);
void StepperMotor_incState(StepperMotor* motor);
unsigned char StepperMotor_powerOn(StepperMotor* motor);
unsigned char StepperMotor_powerOff(StepperMotor* motor);
void StepperMotor_motion_mode(StepperMotor * motor, int mode);
void StepperMotor_step(StepperMotor *motor, int _direction);
void StepperMotor_toggleStepMode(StepperMotor *motor);

unsigned char StepperMotor_faster(StepperMotor* motor);
unsigned char StepperMotor_slower(StepperMotor* motor);
unsigned char StepperMotor_fasterOk(StepperMotor* motor);
unsigned char StepperMotor_slowerOk(StepperMotor* motor);
void StepperMotor_autostep_run(StepperMotor* motor, int _direction);
void StepperMotor_autostep_stop(StepperMotor* motor, unsigned char hold);
void StepperMotor_autostep_speed(StepperMotor *motor, int speed);
unsigned char StepperMotor_autostep_running(StepperMotor *motor);

int StepperMotor_loop(StepperMotor* motor);


#endif