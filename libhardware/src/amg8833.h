#ifndef __amg8833_h__
#define __amg8833_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: amg8833.h
// DESCRIPTION: Driver for Panasonic AMG8833 thermal imaging camera.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include "thermalcamera.h"
#include <stdint.h>

//-----------------------------------
// Documention of basis -------------
//-----------------------------------
// Panasonic Infrared Array Sensor Grid-EYE (AMG88) April 02, 2017
// Panasonic FAQs (Frequently Asked Questions) INFRARED GRIDEYE SENSOR version 1.0
// Panasonic SPECIFICATIONS FOR Infrared Array Sensor August 30, 2011
// https://github.com/adafruit/Adafruit_AMG88xx October 12, 2018

//-----------------------------------
// Device Characteristics -----------
//-----------------------------------
#define AMG8833_PIXEL_TEMP_RESOLUTION_CELSIUS      .25
#define AMG8833_PIXEL_TEMP_MIN_CELSIUS             0.0
#define AMG8833_PIXEL_TEMP_MAX_CELSIUS             80.0
#define AMG8833_PIXEL_TEMP_ERROR_CELSIUS           2.5
#define AMG8833_DEVICE_TEMP_MIN_CELSIUS            -20.0
#define AMG8833_DEVICE_TEMP_MAX_CELSIUS            80.0
#define AMG8833_MAX_RANGE_MILLIMETERS              7000
#define AMG8833_FIELD_OF_VIEW_DEGREES              60.0
#define AMG8833_OPTICAL_AXIS_GAP_DEGREES           5.6
#define AMG8833_CENTER4PIXELS_VIEWANGLEX_DEGREES   7.7
#define AMG8833_CENTER4PIXELS_VIEWANGLEY_DEGREES   8.0

#define AMG8833_VOLTAGE_VOLTS                      3.3
#define AMG8833_CURRENT_NORMAL_MILLIAMP            4.5
#define AMG8833_CURRENT_STANDBY_MILLIAMP           0.8
#define AMG8833_CURRENT_SLEEP_MILLIAMP             0.2

#define AMG8833_NUM_DEVICES   1
#define AMG8833_DEVICEID_1    0

#define AMG8833_ARRAY_WIDTH                  8
#define AMG8833_ARRAY_HEIGHT                 8
#define AMG8833_ARRAY_SIZE                   64

//-----------------------------------
// Driver Constants -----------------
//-----------------------------------


//-----------------------------------
// Driver Data Types ----------------
//-----------------------------------
#define AMG8833_NULL_CONTEXT NULL

typedef int amg8833_id;
typedef int amg8833_callback_id;
struct AMG8833_CONTEXT;
typedef struct AMG8833_CONTEXT *amg8833_context;
typedef float amg8833_framedata_buffer [AMG8833_ARRAY_SIZE];
typedef void (*amg8833_framedata_callback)(amg8833_context ctx, amg8833_callback_id, amg8833_framedata_buffer return_buffer);

//-----------------------------------
// Driver Functions and Inputs ------
//-----------------------------------
amg8833_context amg8833_open(amg8833_id device_id);
void amg8833_close(amg8833_context context);
void amg8833_info(amg8833_context amg8833);

//Driver run / stop
#define AMG8833_NUMFRAMES_ALL          0
#define AMG8833_NUMFRAMES_SINGLESHOT   1
int   amg8833_run(amg8833_context amg8833, int num_frames);
void  amg8833_stop(amg8833_context amg8833, bool stop_framedata);

//Driver output observers
#define AMG8833_OUTPUTUNITS_CELSUIS   0
#define AMG8833_OUTPUTUNITS_FARENHEIT 1
void  amg8833_cfg_output_callbk_framedata(amg8833_context ctx, amg8833_callback_id, amg8833_framedata_callback callbk, amg8833_framedata_buffer frame_buffer);
void amg8833_cfg_output_units(amg8833_context ctx, unsigned char output_units);

//frame rate config and info
#define AMG8833_FRAMESPERSEC                 10
#define AMG8833_10XOVERSAMPLE_FRAMESPERSEC   1
int amg8833_inf_frame_rate(amg8833_context ctx);
void amg8833_cfg_frame_rate(amg8833_context ctx, int frame_rate);

//moving average config and info
#define AMG8833_MOVING_AVERAGE_DISABLED   0
#define AMG8833_MOVING_AVERAGE_ENABLED    1
int amg8833_inf_moving_average(amg8833_context ctx);
void amg8833_cfg_moving_average(amg8833_context ctx, int state);

//interupt config and info
#define AMG8833_INT_MODE_DIFFERENCE        0x00
#define AMG8833_INT_MODE_ABSOLUTE          0x01
bool  amg8833_inf_interrupts_enabled(amg8833_context ctx);
int   amg8833_inf_interrupt_mode(amg8833_context ctx);

//misc driver info
float amg8833_inf_device_temperature(amg8833_context ctx);

#endif