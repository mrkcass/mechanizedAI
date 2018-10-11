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

#define THERMCAM_FRAMEBUFFER_MAX_SIZE 64

typedef int thermcam_id;
struct THERMCAM_CONTEXT;
typedef struct THERMCAM_CONTEXT* thermcam_context;

typedef int thermcam_diagnosticid;

typedef float THERMCAM_FRAMEDATA_BUFFER[THERMCAM_FRAMEBUFFER_MAX_SIZE];
typedef void (*THERM_FRAMEDATA_CALLBACK)(thermcam_context ctx, THERMCAM_FRAMEDATA_BUFFER return_buffer);

thermcam_context thermcam_open(thermcam_id id);
void thermcam_close(thermcam_context camt);
void thermcam_info(thermcam_context camt);
int thermcam_context_to_id(thermcam_context camt);
int thermcam_diagnostics(thermcam_context camt, thermcam_diagnosticid output_format);
int thermcam_imager_width(thermcam_context camt);
int thermcam_imager_height(thermcam_context camt);
float thermcam_imager_resolution_celsius(thermcam_context camt);
float thermcam_imager_range_min_celsius(thermcam_context camt);
float thermcam_imager_range_max_celsius(thermcam_context camt);
int thermcam_imager_range_max_milimeters(thermcam_context camt);
float thermcam_imager_field_of_view_degrees(thermcam_context camt);
#endif