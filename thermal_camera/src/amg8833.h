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

#define AMG8833_NUM_DEVICES   1
#define AMG8833_DEVICEID_1    0

#define AMG8833_ARRAY_WIDTH                  8
#define AMG8833_ARRAY_HEIGHT                 8
#define AMG8833_ARRAY_SIZE                   64

#define AMG8833_FRAMES_PER_SECOND            10

#define AMG8833_PIXEL_TEMP_RESOLUTION_CELSIUS   .25
#define AMG8833_PIXEL_TEMP_MIN_CELSIUS          0.0
#define AMG8833_PIXEL_TEMP_MAX_CELSIUS          80.0
#define AMG8833_MAX_RANGE_MILLIMETERS           7000
#define AMG8833_FIELD_OF_VIEW_DEGREES           60.0
#define AMG8833_OPTICAL_AXIS_GAP_DEGREES        5.6
#define AMG8833_CENTER4PIXELS_VIEWANGLEX        7.7
#define AMG8833_CENTER4PIXELS_VIEWANGLEY        8.0

#define AMG8833_NULL_CONTEXT NULL

typedef int amg8833_id;
struct AMG8833_CONTEXT;
typedef struct AMG8833_CONTEXT *amg8833_context;
typedef float AMG8833_FRAMEDATA_BUFFER [AMG8833_ARRAY_SIZE];
typedef void (*AMG8833_FRAMEDATA_CALLBACK)(amg8833_context ctx, AMG8833_FRAMEDATA_BUFFER return_buffer);

amg8833_context amg8833_open(amg8833_id device_id);
void amg8833_close(amg8833_context context);
void amg8833_info(amg8833_context amg8833);

#define AMG8833_NUMFRAMES_ALL          0
#define AMG8833_NUMFRAMES_SINGLESHOT   1
int   amg8833_run(amg8833_context amg8833, int num_frames);
void  amg8833_stop(amg8833_context amg8833, bool stop_framedata);
void  amg8833_output_callbk_framedata(amg8833_context ctx, AMG8833_FRAMEDATA_CALLBACK callbk, AMG8833_FRAMEDATA_BUFFER frame_buffer);

float amg8833_device_temperature(amg8833_context ctx);


#define AMG8833_INT_MODE_DIFFERENCE        0x00
#define AMG8833_INT_MODE_ABSOLUTE          0x01
bool  amg8833_interrupts_enabled(amg8833_context ctx);
int   amg8833_interrupt_mode(amg8833_context ctx);

amg8833_id amg8833_context_to_id(amg8833_context amg8833);

#endif