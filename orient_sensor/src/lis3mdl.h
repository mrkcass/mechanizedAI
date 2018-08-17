#ifndef __lis3mdl_h__
#define __lis3mdl_h__

#include "mraa.h"
#include "mraa/i2c.h"

int lis3_init(mraa_i2c_context i2c, int bus);
int lis3_sample(mraa_i2c_context i2c, float * sample_buff_mag);

#endif