#ifndef __lsm6ds33_h__
#define __lsm6ds33_h__

#include "mraa.h"
#include "mraa/i2c.h"

int ds33_init(mraa_i2c_context i2c, int bus);
int ds33_sample(mraa_i2c_context i2c, float * sample_buff_accel, float * sample_buff_gyro);

#endif