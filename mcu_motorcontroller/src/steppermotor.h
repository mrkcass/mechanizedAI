#ifndef __steppermotor_h__
#define __steppermotor_h__

#include "wire.h"

typedef unsigned char bool;
#define false 0
#define true 1

enum
{
	StepperMotor_WIRE_A_POSITVE,
	StepperMotor_WIRE_A_NEGATIVE,
	StepperMotor_WIRE_B_POSITIVE,
	StepperMotor_WIRE_B_NEGATIVE,
};
#define StepperMotor_WIRE_COUNT 4
enum
{
	StepperMotor_MOTORSTATE_STOPPED,
	StepperMotor_MOTORSTATE_ACCEL,
	StepperMotor_MOTORSTATE_CRUISE,
	StepperMotor_MOTORSTATE_DECEL,
	StepperMotor_MOTORSTATE_DECEL_STOP,
};

#define MOTORMODE_STEP 0
#define MOTORMODE_AUTOSTEP 1

#define StepperMotor_DIR_CCW 1
#define StepperMotor_DIR_NONE 0
#define StepperMotor_DIR_CW -1

typedef bool WIRE_ARRAY[StepperMotor_WIRE_COUNT];

typedef struct _StepperMotor_
{
	int power;
	int mode;
	int speed;
	int acceleration;
	int wireState;
	int motorState;
	int direction;
	bool hold;
	Wire wires[StepperMotor_WIRE_COUNT];
	const WIRE_ARRAY * wireStateMap;
} StepperMotor;

void StepperMotor_StepperMotor(StepperMotor* motor, int pinApositive, int pinAnegaitve, int pinBpositive, int pinBnegaitve);
void StepperMotor_initWires(StepperMotor* motor, int pinApositive, int pinAnegaitve, int pinBpositive, int pinBnegaitve);
void StepperMotor_initMotor(StepperMotor* motor);
void StepperMotor_sendState(StepperMotor* motor, const bool wiresStates[StepperMotor_WIRE_COUNT]);
void StepperMotor_incState(StepperMotor* motor);
bool StepperMotor_powerOn(StepperMotor* motor);
bool StepperMotor_powerOff(StepperMotor* motor);
bool StepperMotor_faster(StepperMotor* motor);
bool StepperMotor_slower(StepperMotor* motor);
bool StepperMotor_fasterOk(StepperMotor* motor);
bool StepperMotor_slowerOk(StepperMotor* motor);
bool StepperMotor_running(StepperMotor* motor);
void StepperMotor_run(StepperMotor* motor, int _direction);
void StepperMotor_step(StepperMotor* motor, int _direction);

void StepperMotor_stop(StepperMotor* motor, bool hold);
void StepperMotor_toggleStepMode(StepperMotor* motor);

int StepperMotor_loop(StepperMotor* motor);


#endif