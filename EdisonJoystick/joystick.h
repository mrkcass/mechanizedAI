#ifndef __joystick_h__
#define __joystick_h__

#include "wire.h"

#define JOYSTICK_SRC_AXIS_X 1
#define JOYSTICK_SRC_AXIS_Y 2
#define JOYSTICK_SRC_BUTTON 3

#define JOYSTICK_RANGE 1000

#define JOYSTICK_BTN_ANLOG_THRESHOLD 650 

struct Joystick
{

	Joystick();
	~Joystick();
	int connect(int x_axis_pin, int y_axis_pin, int button_pin, bool pull_up);
	int calibrate(int num_levels);
	int read(int source);

	int x_val;
	int y_val;
	int button_val;

private:
	bool calibrate_range(int axis, int sample, int * min, int * max);
	bool calibrate_center(int * x_center_min, int * x_center, int * x_center_max,
		                 int * y_center_min, int * y_center, int * y_center_max);
	int calibrate_demo(int axis, int last_power, int center_min, int center, int center_max, int power_range_lo, int power_range_hi, int num_power_levels);

	int calc_power(int axis, int raw_val);
	int calc_linear_power(int val, int center_min, int center, int center_max, int power_range_lo, int power_range_hi, int num_power_levels);

	int read_raw(int source);


	Wire * x_wire;
	Wire * y_wire;
	Wire * button_wire;

	int num_power_levels;
	int x_min, x_max, x_center_min, x_center, x_center_max, x_power_range_lo, x_power_range_hi;
	int y_min, y_max, y_center_min, y_center, y_center_max, y_power_range_lo, y_power_range_hi;
};

#endif
