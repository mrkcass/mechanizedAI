#include <pthread.h>
#include "mraa/gpio.h"
#include "unistd.h"

#include "wire.h"
#include "steppermotor.h"


static void * loop(void * data);

StepperMotor::StepperMotor(int pinApositive, int pinAnegaitve, int pinBpositive, int pinBnegaitve)
{
	initWires(pinApositive, pinAnegaitve, pinBpositive, pinBnegaitve);
	power = POWER_OFF;
	state = 0;
}

void StepperMotor::initWires(int pinApositive, int pinAnegaitve, int pinBpositive, int pinBnegaitve)
{
	wires[WIRE_A_POSITVE].connect(pinApositive);
	wires[WIRE_A_NEGATIVE].connect(pinAnegaitve);
	wires[WIRE_B_POSITIVE].connect(pinBpositive);
	wires[WIRE_B_NEGATIVE].connect(pinBnegaitve);
}

void StepperMotor::initMotor()
{
	//init motor winding A
	wires[WIRE_A_POSITVE].write(LOW);
	wires[WIRE_A_NEGATIVE].write(LOW);

	wires[WIRE_B_POSITIVE].write(LOW);
	wires[WIRE_B_NEGATIVE].write(LOW);
}

void StepperMotor::sendState(const bool wiresStates[WIRE_COUNT])
{
	wires[WIRE_A_POSITVE].write(wiresStates[0] ? HIGH : LOW);
	wires[WIRE_A_NEGATIVE].write(wiresStates[1] ? HIGH : LOW);
	wires[WIRE_B_POSITIVE].write(wiresStates[2] ? HIGH : LOW);
	wires[WIRE_B_NEGATIVE].write(wiresStates[3] ? HIGH : LOW);
}

void StepperMotor::incState()
{
	state += direction;
	if (state < 0)
		state = NUM_MOTOR_STATES - 1;
	else if (state >= NUM_MOTOR_STATES)
		state = 0;
}

bool StepperMotor::powerOn()
{
	bool poweredOn = true;
	initMotor();
	power = POWER_ON;
	direction = DIR_NONE;
	mode = MODE_IDLE;
	speed = slowest;
	state = 0;
	//start the motor thread. plays motor scripts.
	int iret1 = pthread_create(&loopThread, NULL, loop, (void*)this);
	if (iret1)
	{
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret1);
		poweredOn = false;
	}
	return poweredOn;
}

void StepperMotor::powerOff()
{
	power = POWER_DOWN;
	while (power != POWER_OFF)
		usleep(1000);
	sendState(STATE_IDLE);
	direction = DIR_NONE;
	mode = MODE_IDLE;
	speed = slowest;
	state = 0;
}

void StepperMotor::faster()
{
	speed -= rate;
}

void StepperMotor::slower()
{
	speed += rate;
}

bool StepperMotor::fasterOk()
{
	if (speed > fastest)
		return true;
	else
		return false;
}

bool StepperMotor::slowerOk()
{
	if (speed < slowest)
		return true;
	else
		return false;
}

bool StepperMotor::running()
{
	return mode == MODE_RUN;
}

void StepperMotor::run(int _direction)
{
	speed = slowest;
	direction = _direction;
	mode = MODE_RUN;
}

void StepperMotor::stop(bool hold)
{
	direction = DIR_NONE;
	if (hold)
		mode = MODE_HOLD;
	else
		mode = MODE_IDLE;
}

void StepperMotor::toggleStepMode()
{
	if (STATES == STATES_HALFSTEP)
		STATES = STATES_ONESTEP;
	else if (STATES == STATES_ONESTEP)
		STATES = STATES_TWOPHASE;
	else if (STATES == STATES_TWOPHASE)
		STATES = STATES_HALFSTEP;
}

static void * loop(void * data)
{
	StepperMotor * motor = (StepperMotor*)data;

	motor->state = 0;

	while (motor->power == StepperMotor::POWER_ON)
	{
		usleep(motor->speed);
		if (motor->mode == StepperMotor::MODE_IDLE)
			motor->sendState(motor->STATE_IDLE);
		else
			motor->sendState(motor->STATES[motor->state]);
		motor->incState();
	}
	motor->power = StepperMotor::POWER_OFF;
	return NULL;
}