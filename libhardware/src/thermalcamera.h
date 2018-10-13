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

#define THERMCAM_DIAGNOSTICID_FRAMEDATA_CONSOLE_OUTPUT         1
#define THERMCAM_DIAGNOSTICID_FRAMEDATA_DEVTEMP_CONSOLE_OUTPUT 2

#define THERMCAM_FRAMEBUFFER_MAX_SIZE 64

#define THERMCAM_OUTPUTUNITS_CELSUIS   0
#define THERMCAM_OUTPUTUNITS_FARENHEIT 1

typedef int thermcam_id;
struct THERMCAM_CONTEXT;
typedef struct THERMCAM_CONTEXT* thermcam_context;

typedef int thermcam_diagnosticid;

typedef float thermcam_framedata_buffer[THERMCAM_FRAMEBUFFER_MAX_SIZE];
typedef int thermcam_observer_id;
typedef void (*thermcam_framedata_callback)(thermcam_context ctx, thermcam_observer_id observer_id, thermcam_framedata_buffer return_buffer);
typedef void (*thermcam_framedata_devicetemp_callback)(thermcam_context ctx, thermcam_observer_id observer_id, thermcam_framedata_buffer return_buffer, float device_temp);

thermcam_context thermcam_open(thermcam_id id);
void  thermcam_close(thermcam_context camt);

int   thermcam_diagnostics(thermcam_context camt, thermcam_diagnosticid diag_id);
void  thermcam_info(thermcam_context camt);

#define THERMCAM_NUMFRAMES_CONTINUOUS   0
#define THERMCAM_NUMFRAMES_SINGLESHOT   1
int   thermcam_run(thermcam_context camt, int num_frames);
void  thermcam_cfg_observer_framedata(
            thermcam_context camt,
            thermcam_observer_id observer_id,
            thermcam_framedata_callback observer,
            thermcam_framedata_buffer * observer_buffer);
void  thermcam_cfg_observer_framedata_devicetemp(
            thermcam_context camt,
            thermcam_observer_id observer_id,
            thermcam_framedata_devicetemp_callback observer,
            thermcam_framedata_buffer * observer_buffer);

void thermcam_stop(thermcam_context camt);

void  thermcam_cfg_output_units(thermcam_context ctx, unsigned char output_units);

int   thermcam_inf_imager_width(thermcam_context camt);
int   thermcam_inf_imager_height(thermcam_context camt);
float thermcam_inf_imager_resolution_celsius(thermcam_context camt);
float thermcam_inf_imager_range_min_celsius(thermcam_context camt);
float thermcam_inf_imager_range_max_celsius(thermcam_context camt);
int   thermcam_inf_imager_range_max_milimeters(thermcam_context camt);
float thermcam_inf_imager_field_of_view_degrees(thermcam_context camt);

int   thermcam_context_to_id(thermcam_context camt);
#endif