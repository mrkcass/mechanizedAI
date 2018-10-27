#ifndef __ahrs_h__
#define __ahrs_h__

#include "somax.h"

#define AHRS_NUM_DEVICES   2
#define AHRS_ID_FRAME      1
#define AHRS_ID_CAMD       2

#define AHRS_NUM_SENSORS   9

#define AHRS_NUM_AXIS      3
#define AHRS_AXIS_HEADING  0
#define AHRS_AXIS_PITCH    1
#define AHRS_AXIS_ROLL     2

#define AHRS_CONTEXT_NULL 0

#define AHRS_OUTPUTFORMAT_EULER           1
#define AHRS_OUTPUTFORMAT_QUATERNION      2
#define AHRS_OUTPUTFORMAT_MAGNETOMETER    4

#define AHRS_RUNMODE_AHRS           0
#define AHRS_RUNMODE_IMU            1

struct AHRS_CONTEXT;

typedef int ahrs_id;
typedef struct AHRS_CONTEXT* ahrs_context;
typedef void (*AHRS_EULER_CALLBACK)(ahrs_context ahrs, float heading, float pitch, float roll);
typedef void (*AHRS_QUATERNION_CALLBACK)(ahrs_context ahrs, float w, float x, float y, float z);
typedef void (*AHRS_MAGNETOMETER_CALLBACK)(ahrs_context ahrs, int mag_x, int mag_y, int mag_z);

ahrs_context ahrs_open(ahrs_id id);
void ahrs_close();
void ahrs_info(ahrs_context ahrs);
int ahrs_context_to_id(ahrs_context ahrs);
int ahrs_cfg_run_mode(ahrs_context ahrs, smx_byte mode);
int ahrs_test(int output_format);
int ahrs_run(AHRS_EULER_CALLBACK euler_callbk, AHRS_QUATERNION_CALLBACK quaternion_callbk, AHRS_MAGNETOMETER_CALLBACK magnetometer_callbk);

void ahrs_quaternion_to_euler(const float qw, const float qx, const float qy, const float qz, float  * ex, float  * ey, float * ez);

#endif