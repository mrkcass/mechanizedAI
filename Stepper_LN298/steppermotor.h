#ifndef __steppermotor_h__
#define __steppermotor_h__

#include "wire.h"

struct StepperMotor
{
	enum
	{
		WIRE_A_POSITVE,
		WIRE_A_NEGATIVE,
		WIRE_B_POSITIVE,
		WIRE_B_NEGATIVE,
		WIRE_COUNT,
	};
	enum
	{
		MODE_IDLE,
		MODE_STEP,
		MODE_RUN,
		MODE_STOP,
		MODE_HOLD,
	};

	static const int LOW = 0;
	static const int HIGH = 1;
	static const int POWER_OFF = 0;
	static const int POWER_DOWN = -1;
	static const int POWER_ON = 1;

	static const int DIR_CCW = 1;
	static const int DIR_NONE = 0;
	static const int DIR_CW = -1;

	static const int NUM_MOTOR_STATES = 8;

	const bool STATES_HALFSTEP[NUM_MOTOR_STATES][WIRE_COUNT] =
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
	const bool STATES_ONESTEP[NUM_MOTOR_STATES][WIRE_COUNT] =
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
	const bool STATES_TWOPHASE[NUM_MOTOR_STATES][WIRE_COUNT] =
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
	const bool STATE_IDLE[WIRE_COUNT] =
	{
		false, false, false, false,
	};
	typedef bool WIRE_ARRAY[WIRE_COUNT];

	const WIRE_ARRAY * STATES = STATES_ONESTEP;
	static const int slowest = 2000 + 1200;
	static const int fastest = 2000 - 500;
	static const int rate = 100;

	StepperMotor(int pinApositive, int pinAnegaitve, int pinBpositive, int pinBnegaitve);
	void initWires(int pinApositive, int pinAnegaitve, int pinBpositive, int pinBnegaitve);
	void initMotor();
	void sendState(const bool wiresStates[WIRE_COUNT]);
	void incState();
	bool powerOn();
	void powerOff();
	void faster();
	void slower();
	bool fasterOk();
	bool slowerOk();
	bool running();
	void run(int _direction);
	void stop(bool hold);
	void toggleStepMode();

	int power;
	int speed;
	int state;
	int mode;
	int direction = DIR_NONE;
	struct Wire wires[WIRE_COUNT];
	pthread_t loopThread;

};


#endif