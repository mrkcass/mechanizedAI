#include <pthread.h>
#include <stdio.h>
#include "unistd.h"
#include "stdlib.h"
#include "memory.h"


#include "joystick.h"

enum
{
	RUNSTATE_RUN,
	RUNSTATE_RUNNING,
	RUNSTATE_STOP,
	RUNSTATE_STOPPING,
	RUNSTATE_STOPPPED,
};

Joystick::Joystick()
{
	x_wire = y_wire = button_wire = NULL;
	num_power_levels = 1;
	x_center_min = x_center = x_center_max = x_power_range_lo = x_power_range_hi = 512;
	y_center_min = y_center = y_center_max = y_power_range_lo = y_power_range_hi = 512;
}

Joystick::~Joystick()
{
	if (x_wire)
		delete x_wire;
	if (y_wire)
		delete y_wire;
	if (button_wire)
		delete button_wire;
}

int Joystick::connect(int x_axis_pin, int y_axis_pin, int button_pin, bool pull_up)
{
	x_wire = new Wire();
	x_wire->connectAnalog(x_axis_pin);
	y_wire = new Wire();
	y_wire->connectAnalog(y_axis_pin);
	button_wire = new Wire();
	//  todo: trouble connecting the edison to io 18 / 19 as digital input
	//        for now using the analog input and scaling the value to 0 or 1
	button_wire->connectAnalog(button_pin);
	return 0;
}

int Joystick::read(int source)
{
	if (source == JOYSTICK_SRC_AXIS_X || source == JOYSTICK_SRC_AXIS_Y)
		return calc_power(source, read_raw(source));
	else if (source == JOYSTICK_SRC_BUTTON)
		return read_raw(source) > JOYSTICK_BTN_ANLOG_THRESHOLD ? 1  : 0;

	return -1;
}

int Joystick::read_raw(int source)
{
	if (source == JOYSTICK_SRC_AXIS_X)
		return x_wire->read();
	else if (source == JOYSTICK_SRC_AXIS_Y)
		return y_wire->read();
	else if (source == JOYSTICK_SRC_BUTTON)
		return button_wire->read();

	return -1;
}

int Joystick::calibrate(int num_levels)
{
	bool button_pressed;
	int x_val, y_val, new_button;
	int calibration_mode;
	int x_power_last, y_power_last;

	int ONE_MILLISECOND = 1000;
	int FIFTY_MILLISECONDS = ONE_MILLISECOND * 50;

	x_min = y_min = 1000000;
	x_max = y_max = 0;
	calibration_mode = 0;
	x_power_last = y_power_last = 0;
	button_pressed = false;

	printf("entering stick calibration mode\n");

	while (calibration_mode != 100)
	{
		x_val = read_raw(JOYSTICK_SRC_AXIS_X);
		y_val = read_raw(JOYSTICK_SRC_AXIS_Y);
		new_button = read(JOYSTICK_SRC_BUTTON);

		printf("x: %03d y: %03d b: % 03d\n", x_val, y_val, new_button);
		//printf("power range y lo: %03d hi: %03d\n", y_power_range_lo, y_power_range_hi);

		if (button_pressed && new_button == 0)
		{
			printf("button released\n");
			button_pressed = false;
			switch (calibration_mode)
			{
				case 1:
					calibrate_center(&x_center_min, &x_center, &x_center_max, &y_center_min, &y_center, &y_center_max);
					num_power_levels = num_levels;
					
					//wiggling the joystick causes noise. agressively modulating the
					//stick while tuning can cause the max extent to be set too lo
					x_power_range_lo = x_center_min - x_min;
					x_power_range_lo -= 25;
					
					//wiggling the joystick causes noise. agressively modulating the
					//stick while tuning can cause the max extent to be set too high
					x_power_range_hi = x_max - x_center_max;
					x_power_range_hi -= 25;

					y_power_range_lo = y_center_min - y_min;
					y_power_range_lo -= 25;
					y_power_range_hi = y_max - y_center_max;
					y_power_range_hi -= 25;
					printf("power range x lo: %03d hi: %03d\n", x_power_range_lo, x_power_range_hi);
					printf("power range y lo: %03d hi: %03d\n", y_power_range_lo, y_power_range_hi);
					calibration_mode = 2;
					break;
			}

		}
		else if (!button_pressed && new_button == 1)
		{
			button_pressed = true;
			calibration_mode++;
		}
		else if (calibration_mode == 0)
		{
			calibrate_range(JOYSTICK_SRC_AXIS_X, x_val, &x_min, &x_max);
			calibrate_range(JOYSTICK_SRC_AXIS_Y, y_val, &y_min, &y_max);
		}
		else if (calibration_mode == 2)
		{
			calibration_mode = 100;
			printf("exiting calibartion.\n");
		}
		usleep(FIFTY_MILLISECONDS);
	}
	return false;
}

bool Joystick::calibrate_range(int axis, int sample, int * min, int * max)
{
	bool changed = false;
	if (sample < *min)
	{
		*min = sample;
		changed = true;
	}
	else if (sample > *max)
	{
		*max = sample;
		changed = true;
	}
	if (changed)
	{
		int center = (*max - *min) / 2;
		if (axis == JOYSTICK_SRC_AXIS_X)
			printf("\tX MIN=%3d CEN=%3d MAX=%3d\n", *min, center, *max);
		else
			printf("\ty min=%3d cen=%3d max=%3d\n", *min, center, *max);
	}
	return changed;
}

bool Joystick::calibrate_center(int * x_center_min, int * x_center, int * x_center_max,
							   int * y_center_min, int * y_center, int * y_center_max)
{
	int ONE_SECOND = 1000 * 1000;

	printf("\nstarting dead stick calibration in 1 second. don't move the stick\n");
	usleep(ONE_SECOND);
	printf("dead stick calibration started....");
	
	int num_samples = 25;
	int sample_time = ONE_SECOND / 2;
	int sample_rate = sample_time / num_samples;
	int sample_sum_x = 0;
	int sample_sum_y = 0;
	int sample_min_x = 10000;
	int sample_min_y = 10000;
	int sample_max_x = 0;
	int sample_max_y = 0;

	for (int counter = 0; counter < num_samples; counter++)
	{
		int sample_x = read_raw(JOYSTICK_SRC_AXIS_X);
		int sample_y = read_raw(JOYSTICK_SRC_AXIS_Y);
		sample_sum_x += sample_x;
		sample_sum_y += sample_y;
		if (sample_x < sample_min_x)
			sample_min_x = sample_x;
		else if (sample_x > sample_max_x)
			sample_max_x = sample_x;
		if (sample_y < sample_min_y)
			sample_min_y = sample_y;
		else if (sample_y > sample_max_y)
			sample_max_y = sample_y;
		usleep(sample_rate);
		printf(".");
		fflush(stdout);
	}
	printf("\n");

	*x_center = sample_sum_x / num_samples;
	*x_center_min = sample_min_x;
	*x_center_max = sample_max_x;

	*y_center = sample_sum_y / num_samples;
	*y_center_min = sample_min_y;
	*y_center_max = sample_max_y;

	printf("X min_center = %3d, dead_center = %3d , max_center = %3d\n", *x_center_min, *x_center, *x_center_max);
	printf("Y min_center = %3d, dead_center = %3d , max_center = %3d\n", *y_center_min, *y_center, *y_center_max);
	printf("\ndead stick calibration complete. Passed(HNC)\n");

	return true;
}



int Joystick::calibrate_demo(int axis, int last_power, int center_min, int center, int center_max, int power_range_lo, int power_range_hi, int num_power_levels)
{
	int power;
	int val;
	char x_axis_prompt[16] = "x-axis";
	char y_axis_prompt[16] = "y-axis";
	char * prompt;

	if (axis == JOYSTICK_SRC_AXIS_X)
		prompt = x_axis_prompt;
	else
		prompt = y_axis_prompt;

	val = read_raw(axis);

	power = calc_linear_power(val, center_min, center, center_max, power_range_lo, power_range_hi, num_power_levels);

	if (power != last_power)
		printf("%s val = %3d power = %3d\n", prompt, val, power);

	return power;
}

int Joystick::calc_power(int axis, int raw_val)
{
	if (axis == JOYSTICK_SRC_AXIS_X)
		return calc_linear_power(raw_val, x_center_min, x_center, x_center_max, x_power_range_lo, x_power_range_hi, num_power_levels);
	else
		return calc_linear_power(raw_val, y_center_min, y_center, y_center_max, y_power_range_lo, y_power_range_hi, num_power_levels);
}

int Joystick::calc_linear_power(int val, int center_min, int center, int center_max, int power_range_lo, int power_range_hi, int num_power_levels)
{
	int power = 0;

	if (val > center)
	{
		if (val <= center_max)
		{
			power = 0;
		}
		else
		{
			power = (val - center_max) / (power_range_hi / num_power_levels);
		}
	}
	else
	{
		if (val >= center_min)
		{
			power = 0;
		}
		else
		{
			power = (val - center_min) / (power_range_lo / num_power_levels);
		}
	}

	return power;
}



