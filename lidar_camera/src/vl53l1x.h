#ifndef __vl53l1x_h__
#define __vl53l1x_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: vl53l1x.h
// DESCRIPTION: Driver for ST VL53L1X LIDAR imaging camera.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include "lidarcamera.h"
#include <stdint.h>

//-----------------------------------
// Documention of basis -------------
//-----------------------------------
// ST VL53L1X datasheet. DocID031281 Rev 2. 2/2018
// ST VL53L1X user manual. DocID031478 Rev 2. 6/2018
// https://github.com/sparkfun/SparkFun_VL53L1X_Arduino_Library. October 20, 2018

//-----------------------------------
// Device Characteristics -----------
//-----------------------------------
#define VL53L1X_MIN_RANGE_MILLIMETERS              40
#define VL53L1X_MAX_RANGE_MILLIMETERS              4000
#define VL53L1X_FIELD_OF_VIEW_DEGREES              27.0
#define VL53L1X_MAX_REFRESH_RATE_FPS               50.0
#define VL53L1X_LASER_FREQUENCY_NANOMETERS         940
#define VL53L1X_OPTICAL_AXIS_GAP_DEGREES           5.6
#define VL53L1X_CENTER4PIXELS_VIEWANGLEX_DEGREES   7.7
#define VL53L1X_CENTER4PIXELS_VIEWANGLEY_DEGREES   8.0

#define VL53L1X_VOLTAGE_VOLTS                      3.3
#define VL53L1X_CURRENT_NORMAL_MILLIAMP            16.0
#define VL53L1X_CURRENT_STANDBY_MILLIAMP           0.9
#define VL53L1X_CURRENT_SLEEP_MILLIAMP             0.7

#define VL53L1X_NUM_DEVICES   1
#define VL53L1X_DEVICEID_1    0

#define VL53L1X_ARRAY_WIDTH                  16
#define VL53L1X_ARRAY_HEIGHT                 16
#define VL53L1X_ARRAY_SIZE                   256

//-----------------------------------
// Driver Constants -----------------
//-----------------------------------


//-----------------------------------
// Driver Data Types ----------------
//-----------------------------------
#define VL53L1X_NULL_CONTEXT NULL

typedef int vl53l1x_id;
typedef int vl53l1x_callback_id;
struct VL53L1X_CONTEXT;
typedef struct VL53L1X_CONTEXT *vl53l1x_context;
typedef float vl53l1x_framedata_buffer [VL53L1X_ARRAY_SIZE];
typedef void (*vl53l1x_framedata_callback)(vl53l1x_context ctx, vl53l1x_callback_id, vl53l1x_framedata_buffer return_buffer);

//-----------------------------------
// Driver Functions and Inputs ------
//-----------------------------------
vl53l1x_context vl53l1x_open(vl53l1x_id device_id);
void vl53l1x_close(vl53l1x_context context);
void vl53l1x_info(vl53l1x_context vl53l1x);

//Driver run / stop
#define VL53L1X_NUMFRAMES_ALL          0
#define VL53L1X_NUMFRAMES_SINGLESHOT   1
int   vl53l1x_run(vl53l1x_context vl53l1x, int num_frames);
void  vl53l1x_stop(vl53l1x_context vl53l1x, bool stop_framedata);

//Driver output observers
void  vl53l1x_cfg_output_callbk_framedata(vl53l1x_context ctx, vl53l1x_callback_id, vl53l1x_framedata_callback callbk, vl53l1x_framedata_buffer frame_buffer);

//frame rate config and info
#define VL53L1X_FRAMESPERSEC_5               5
#define VL53L1X_FRAMESPERSEC_10              10
#define VL53L1X_FRAMESPERSEC_15              15
#define VL53L1X_FRAMESPERSEC_30              30
#define VL53L1X_FRAMESPERSEC_50              50
#define VL53L1X_10XOVERSAMPLE_FRAMESPERSEC   1
int vl53l1x_inf_frame_rate(vl53l1x_context ctx);
void vl53l1x_cfg_frame_rate(vl53l1x_context ctx, int frame_rate);

#endif