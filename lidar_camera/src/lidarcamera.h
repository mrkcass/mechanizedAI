#ifndef __lidarcam_h__
#define __lidarcam_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: lidarcamera.h
// DESCRIPTION: Somax LIDAR imaging interface.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#define LIDARCAM_NUM_CAMERAS  1
#define LIDARCAMID_GIMBAL     1

#define LIDARCAM_CONTEXT_NULL 0

#define LIDARCAM_DIAGNOSTICID_FRAMEDATA_CONSOLE_OUTPUT         1
#define LIDARCAM_DIAGNOSTICID_FRAMEDATA_DEVTEMP_CONSOLE_OUTPUT 2

#define LIDARCAM_FRAMEBUFFER_MAX_SIZE 64

typedef int lidarcam_id;
struct LIDARCAM_CONTEXT;
typedef struct LIDARCAM_CONTEXT* lidarcam_context;

typedef int lidarcam_diagnosticid;

typedef float lidarcam_framedata_buffer[LIDARCAM_FRAMEBUFFER_MAX_SIZE];
typedef int lidarcam_observer_id;
typedef void (*lidarcam_framedata_callback)(lidarcam_context ctx, lidarcam_observer_id observer_id, lidarcam_framedata_buffer return_buffer);

lidarcam_context lidarcam_open(lidarcam_id id);
void  lidarcam_close(lidarcam_context caml);

int   lidarcam_diagnostics(lidarcam_context caml, lidarcam_diagnosticid diag_id);
void  lidarcam_info(lidarcam_context caml);

#define LIDARCAM_NUMFRAMES_CONTINUOUS   0
#define LIDARCAM_NUMFRAMES_SINGLESHOT   1
int   lidarcam_run(lidarcam_context caml, int num_frames);
void  lidarcam_cfg_observer_framedata(
            lidarcam_context caml,
            lidarcam_observer_id observer_id,
            lidarcam_framedata_callback observer,
            lidarcam_framedata_buffer * observer_buffer);

void lidarcam_stop(lidarcam_context caml);

void  lidarcam_cfg_output_units(lidarcam_context ctx, unsigned char output_units);

int   lidarcam_inf_imager_width(lidarcam_context caml);
int   lidarcam_inf_imager_height(lidarcam_context caml);
float lidarcam_inf_imager_resolution_celsius(lidarcam_context caml);
float lidarcam_inf_imager_range_min_celsius(lidarcam_context caml);
float lidarcam_inf_imager_range_max_celsius(lidarcam_context caml);
int   lidarcam_inf_imager_range_max_milimeters(lidarcam_context caml);
float lidarcam_inf_imager_field_of_view_degrees(lidarcam_context caml);

int   lidarcam_context_to_id(lidarcam_context caml);
#endif