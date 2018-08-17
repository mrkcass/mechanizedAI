#include <stdio.h>
#include <stdlib.h>

#if 0
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
		{ true , false, false, true  },
		{ true , false, false, false },
		{ true , true , true , false },
		{ false, true , true , false },
		{ false, true , true , false },
		{ false, true , true , true  },
		{ false, false, false, true  },
		{ true , false, false, true  },
	};
	const bool STATES_ONESTEP[NUM_MOTOR_STATES][WIRE_COUNT] =
	{
		//single phase stepping
		// a+     a-     b+     b-
		{ true,  false, false, false },
		{ false, false, true,  false },
		{ false, true,  false, false },
		{ false, false, false, true	 },
		{ true,  false, false, false },
		{ false, false, true,  false },
		{ false, true,  false, false },
		{ false, false, false, true  },
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

	StepperMotor(int pinApositive, int pinAnegaitve, int pinBpositive, int pinBnegaitve)
	{
		initWires(pinApositive, pinAnegaitve, pinBpositive, pinBnegaitve);
		power = POWER_OFF;
		state = 0;
	}
	void initWires(int pinApositive, int pinAnegaitve, int pinBpositive, int pinBnegaitve)
	{
		wires[WIRE_A_POSITVE].connect(pinApositive);
		wires[WIRE_A_NEGATIVE].connect(pinAnegaitve);
		wires[WIRE_B_POSITIVE].connect(pinBpositive);
		wires[WIRE_B_NEGATIVE].connect(pinBnegaitve);
	}
	void initMotor()
	{
		//init motor winding A
		wires[WIRE_A_POSITVE].write(LOW);
		wires[WIRE_A_NEGATIVE].write(LOW);

		wires[WIRE_B_POSITIVE].write(LOW);
		wires[WIRE_B_NEGATIVE].write(LOW);
	}
	static void * loop(void * data)
	{
		StepperMotor * motor = (StepperMotor*)data;

		motor->state = 0;

		while (motor->power == POWER_ON)
		{
			usleep(motor->speed);
			if (motor->mode == MODE_IDLE)
				motor->sendState(motor->STATE_IDLE);
			else
				motor->sendState(motor->STATES[motor->state]);
			motor->incState();
		}
		motor->power = POWER_OFF;
		return NULL;
	}
	void sendState(const bool wiresStates[WIRE_COUNT])
	{
		wires[WIRE_A_POSITVE].write(wiresStates[0] ? HIGH : LOW);
		wires[WIRE_A_NEGATIVE].write(wiresStates[1] ? HIGH : LOW);
		wires[WIRE_B_POSITIVE].write(wiresStates[2] ? HIGH : LOW);
		wires[WIRE_B_NEGATIVE].write(wiresStates[3] ? HIGH : LOW);
	}
	void incState()
	{
		state += direction;
		if (state < 0)
			state = NUM_MOTOR_STATES - 1;
		else if (state >= NUM_MOTOR_STATES)
			state = 0;
	}
	void powerOn()
	{
		initMotor();
		power = POWER_ON;
		direction = DIR_NONE;
		mode = MODE_IDLE;
		speed = slowest;
		state = 0;
		//start the motor thread. plays motor scripts.
		int iret1 = pthread_create(&loopThread, NULL, StepperMotor::loop, (void*)this);
		if (iret1)
		{
			fprintf(stderr, "Error - pthread_create() return code: %d\n", iret1);
			exit(-1);
		}
	}
	void powerOff()
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
	void faster()
	{
		speed -= rate;
	}
	void slower()
	{
		speed += rate;
	}
	bool fasterOk()
	{
		if (speed > fastest)
			return true;
		else
			return false;
	}
	bool slowerOk()
	{
		if (speed < slowest)
			return true;
		else
			return false;
	}
	bool running()
	{
		return mode == MODE_RUN;
	}
	void run(int _direction)
	{
		speed = slowest;
		direction = _direction;
		mode = MODE_RUN;
	}
	void stop(bool hold)
	{
		direction = DIR_NONE;
		if (hold)
			mode = MODE_HOLD;
		else
			mode = MODE_IDLE;
	}
	void toggleStepMode()
	{
		if (STATES == STATES_HALFSTEP)
			STATES = STATES_ONESTEP;
		else if (STATES == STATES_ONESTEP)
			STATES = STATES_TWOPHASE;
		else if (STATES == STATES_TWOPHASE)
			STATES = STATES_HALFSTEP;
	}

	

	int power;
	int speed;
	int state;
	int mode;
	int direction = DIR_NONE;
	struct Wire wires[WIRE_COUNT];
	pthread_t loopThread;
	
};

#endif

#include "steppermotor.h"

int main(int argc, char * argv[])
{
	mraa_init();

	StepperMotor motor(2,4,7,8);
	motor.powerOn();

	//make terminal send all characters when typed.
	//without this, each terminal command must be
	//followed by enter key being pressed.
	system("/bin/stty raw");
	
	

	while (1)
	{
		int c = getchar();
		printf("\r \r");
		if (c == 'q')
		{
			break;
		}
		else if (c == 'd')
		{
			if (motor.mode == StepperMotor::MODE_IDLE)
			{
				motor.run(StepperMotor::DIR_CW);
			}
			else if (motor.mode == StepperMotor::MODE_RUN)
			{
				if (motor.direction == StepperMotor::DIR_CW)
				{
					if (motor.fasterOk())
					{
						motor.faster();
					}
				}
				else if (motor.direction == StepperMotor::DIR_CCW)
				{
					if (motor.slowerOk())
						motor.slower();
					else if (motor.running())
						motor.stop(false);
				}
			}
		}
		else if (c == 's')
		{
			motor.slower();
		}
		else if (c == 'a')
		{
			if (motor.mode == StepperMotor::MODE_IDLE)
			{
				motor.run(StepperMotor::DIR_CCW);
			}
			else if (motor.mode == StepperMotor::MODE_RUN)
			{
				if (motor.direction == StepperMotor::DIR_CCW)
				{
					if (motor.fasterOk())
						motor.faster();
				}
				else if (motor.direction == StepperMotor::DIR_CW)
				{
					if (motor.slowerOk())
						motor.slower();
					else if (motor.running())
						motor.stop(false);
				}
			}
		}
		else if (c == 'm')
		{
			motor.toggleStepMode();
		}
	}

	motor.powerOff();
	//reset terminal to wait for enter key
	system("/bin/stty cooked");
	
	mraa_deinit();
}