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

#define AMG8833_NUM_DEVICES   1
#define AMG8833_DEVICE_1      0

typedef int amg8833_id;
struct AMG8833_CONTEXT;
typedef struct AMG8833_CONTEXT *amg8833_context;

amg8833_context amg8833_open(amg8833_id id);
void amg8833_close(amg8833_context context);
void amg8833_info(amg8833_context amg8833);
float amg8833_device_temperature(amg8833_context ctx);
bool amg8833_interrupts_enabled(amg8833_context ctx);

#define AMG8833_INT_MODE_DIFFERENCE        0x00
#define AMG8833_INT_MODE_ABSOLUTE          0x01
int amg8833_interrupt_mode(amg8833_context ctx);
//void amg8833_output_callbk_euler(amg8833_context ahrs, AHRS_EULER_CALLBACK euler_callbk);
//void amg8833_output_callbk_quaternion(amg8833_context ahrs, AHRS_QUATERNION_CALLBACK quaternion_callbk);
//void amg8833_output_callbk_magnetometer(amg8833_context ahrs, AHRS_MAGNETOMETER_CALLBACK magnetometer_callbk);
//void amg8833_calibration_status(amg8833_context ahrs, int *calibstat_sys, int *calibstat_gyro, int *calibstat_acc, int *calibstat_mag);
amg8833_id amg8833_context_to_id(amg8833_context amg8833);

#endif