#ifndef __ahrs_h__
#define __ahrs_h__

#include "i2c_interface.h"

#define AHRS_ID_CAMD       1
#define AHRS_ID_FRAME      2
#define AHRS_NUM_DEVICES   2

#define AHRS_NUM_SENSORS   9

#define AHRS_NUM_AXIS      3
#define AHRS_AXIS_HEADING  0
#define AHRS_AXIS_PITCH    1
#define AHRS_AXIS_ROLL     2

#define AHRS_CONTEXT_NULL 0

typedef void (*AHRS_CALLBACK)(int device_id, int axis, int value);
typedef int ahrs_context;

ahrs_context ahrs_open_i2c(int ahrs_id, i2c_context i2c, int i2c_address);
void ahrs_close();
void ahrs_info(ahrs_context ahrs);
int ahrs_test();
int ahrs_run(AHRS_CALLBACK call_on_change);

#endif