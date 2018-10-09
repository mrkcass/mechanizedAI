#ifndef __thermcam_h__
#define __thermcam_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: thermalcamera.h
// DESCRIPTION: Somax thermal imaging interface.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#define THERMCAM_NUM_CAMERAS  1
#define THERMCAMID_GIMBAL     1

#define THERMCAM_CONTEXT_NULL 0

#define THERMCAM_DIAGNOSTICID_CONSOLE_OUTPUT 1

typedef int thermcam_id;
struct THERMCAM_CONTEXT;
typedef struct THERMCAM_CONTEXT* thermcam_context;

typedef int thermcam_diagnosticid;

//typedef void (*AHRS_EULER_CALLBACK)(thermcam_context ahrs, float heading, float pitch, float roll);
//typedef void (*AHRS_QUATERNION_CALLBACK)(thermcam_context ahrs, float w, float x, float y, float z);
//typedef void (*AHRS_MAGNETOMETER_CALLBACK)(thermcam_context ahrs, int mag_x, int mag_y, int mag_z);

thermcam_context thermcam_open(thermcam_id id);
void thermcam_close(thermcam_context camt);
void thermcam_info(thermcam_context camt);
int thermcam_context_to_id(thermcam_context camt);
int thermcam_diagnostics(thermcam_context camt, thermcam_diagnosticid output_format);

#endif