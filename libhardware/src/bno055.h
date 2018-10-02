#ifndef __bno055_h__
#define __bno055_h__

#include "ahrs.h"

#define BNO_NUM_DEVICES    2
#define BNO_DEVICE_CAMD    0
#define BNO_DEVICE_FRAME   1

#define BNO_NUM_AXIS 3
#define BNO_AXIS_X   0
#define BNO_AXIS_Y   1
#define BNO_AXIS_Z   2

#define BNO_SIGN_NEGATIVE    -1
#define BNO_SIGN_SAME        0
#define BNO_SIGN_POSITIVE    1

ahrs_context bno055_open(ahrs_id ahrs);
void bno055_close();
void bno055_configure_axis(ahrs_context ahrs, int axis, int mapped_axis, int mapped_sign);
int  bno055_run();
void bno055_info(ahrs_context ahrs);
void bno055_output_callbk_euler(ahrs_context ahrs, AHRS_EULER_CALLBACK euler_callbk);
void bno055_output_callbk_quaternion(ahrs_context ahrs, AHRS_QUATERNION_CALLBACK quaternion_callbk);
void bno055_output_callbk_magnetometer(ahrs_context ahrs, AHRS_MAGNETOMETER_CALLBACK magnetometer_callbk);
void bno055_calibration_status(ahrs_context ahrs, int *calibstat_sys, int *calibstat_gyro, int *calibstat_acc, int *calibstat_mag);
ahrs_id bno055_context_to_id(ahrs_context ahrs);

#endif