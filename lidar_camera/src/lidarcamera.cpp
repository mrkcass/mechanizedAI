//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: lidarcamera.c
// DESCRIPTION: Somax LIDAR imaging interface.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <stdio.h>
#include <math.h>

#include "somax.h"
#include "lidarcamera.h"
#include "vl53l1x.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define LIDARCAM_MAX_CONTEXTS 16

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA STRUCTURES
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct LIDARCAM_CONTEXT
{
   int context_slot;
   lidarcam_id camera_id;
   vl53l1x_context vl53l1x;
   lidarcam_framedata_buffer * frame_buffer;
   lidarcam_framedata_callback cb_framedata;
   lidarcam_observer_id observer_id;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//todo: this can be extended to map other things to devices other than just
//names. for example open,close and info functions functions .
static const char lidarcam_names[][32] =
    {"LIDARCAM-NULL", "LIDARCAM-GIMBAL[VL53L1X]"};
static int lidarcam_device_id_map[] = {0, VL53L1X_DEVICEID_1};

static int contextlist_used;
static LIDARCAM_CONTEXT context_list[LIDARCAM_MAX_CONTEXTS];
static lidarcam_context lidarcam_diag_context;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool lidarcam_can_open(lidarcam_id id);

static int lidarcam_diag_framedata_toconsole(lidarcam_context lidarcam);
static void lidarcam_diag_toconsole_framedata_callbk(vl53l1x_context ctx, vl53l1x_callback_id callbk_id, vl53l1x_framedata_buffer return_buffer);

static void lidarcam_run_callbk_framedata(vl53l1x_context ctx, vl53l1x_callback_id callbk_id, vl53l1x_framedata_buffer return_buffer);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
lidarcam_context lidarcam_open(lidarcam_id id)
{
   if (!lidarcam_can_open(id))
      return NULL;

   lidarcam_context ctx = &context_list[contextlist_used];
   ctx->context_slot = contextlist_used;
   ctx->camera_id = id;
   ctx->vl53l1x = vl53l1x_open(lidarcam_device_id_map[id]);

   contextlist_used++;

   return ctx;
}

void lidarcam_close(lidarcam_context lidarcam)
{
   vl53l1x_close(lidarcam->vl53l1x);
}

void lidarcam_info(lidarcam_context lidarcam)
{
   printf("LIDARCAM device info: %s -------------\n", lidarcam_names[lidarcam_context_to_id(lidarcam)]);
   vl53l1x_info(lidarcam->vl53l1x);
}

int lidarcam_context_to_id(lidarcam_context lidarcam)
{
   return lidarcam->camera_id;
}

int lidarcam_diagnostics(lidarcam_context lidarcam, lidarcam_diagnosticid diag_id)
{
   return lidarcam_diag_framedata_toconsole(lidarcam);
}

int lidarcam_inf_imager_width(lidarcam_context lidarcam)
{
   return VL53L1X_ARRAY_WIDTH;
}

int lidarcam_inf_imager_height(lidarcam_context lidarcam)
{
   return VL53L1X_ARRAY_HEIGHT;
}

int lidarcam_inf_imager_range_min_millimeters(lidarcam_context lidarcam)
{
   return VL53L1X_MIN_RANGE_MILLIMETERS;
}

int lidarcam_inf_imager_range_max_millimeters(lidarcam_context lidarcam)
{
   return VL53L1X_MAX_RANGE_MILLIMETERS;
}

float lidarcam_inf_imager_field_of_view_degrees(lidarcam_context lidarcam)
{
   return VL53L1X_FIELD_OF_VIEW_DEGREES;
}

//lidarcam_cfg_observer_framedata
//configure the output observer callback function for thermal camera pixel data
//PARAM: lidarcam - thermal camera context.
//PARAM: observer_id -
void lidarcam_cfg_observer_framedata(lidarcam_context lidarcam, lidarcam_observer_id observer_id, lidarcam_framedata_callback observer, lidarcam_framedata_buffer * observer_buffer)
{
   lidarcam->cb_framedata = observer;
   lidarcam->frame_buffer = observer_buffer;
   lidarcam->observer_id = observer_id;
}

// blocking call that runs an observer update loop.
// PARAM: vl53l1x - VL53L1X context
// PARAM: num_frames - number of frames to operate over
// RETURN: 1 if stopped before num_frames were returned. 0 otherwise.
int lidarcam_run(lidarcam_context lidarcam, int num_frames)
{
   if (lidarcam->cb_framedata)
      vl53l1x_cfg_output_callbk_framedata(lidarcam->vl53l1x, lidarcam->context_slot, lidarcam_run_callbk_framedata, *lidarcam->frame_buffer);
   return vl53l1x_run(lidarcam->vl53l1x, num_frames);
}

void lidarcam_stop(lidarcam_context lidarcam)
{
   lidarcam->cb_framedata = NULL;
   lidarcam->frame_buffer = NULL;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool lidarcam_can_open(lidarcam_id id)
{
   if (id <= 0 || id > LIDARCAM_NUM_CAMERAS)
   {
      somax_log_add(SOMAX_LOG_ERR, "LIDARCAM. open id (%d) out of bounds", id);
      return false;
   }
   if (contextlist_used >= LIDARCAM_MAX_CONTEXTS)
   {
      somax_log_add(SOMAX_LOG_ERR, "LIDARCAM. opening context (%d). max contexts already open", id);
      return false;
   }

   return true;
}

static int lidarcam_diag_framedata_toconsole(lidarcam_context lidarcam)
{
   lidarcam_diag_context = lidarcam;
   vl53l1x_cfg_output_callbk_framedata(lidarcam->vl53l1x, lidarcam->context_slot, lidarcam_diag_toconsole_framedata_callbk, (float*)NULL);

   vl53l1x_run(lidarcam->vl53l1x, VL53L1X_NUMFRAMES_ALL);

   return 0;
}

static void lidarcam_diag_toconsole_framedata_callbk(vl53l1x_context ctx, vl53l1x_callback_id callbk_id, vl53l1x_framedata_buffer return_buffer)
{
   printf("Distance = %4.0f\r", return_buffer[0]);
   //todo: convert this to call the run callback so that the callback functionality is also tested.
   // static unsigned long frame_count;

   // frame_count++;

   // if (frame_count % 10 && frame_count != 1)
   //    return;

   // printf("FRAME %04lu --------------------------------------------------\n", frame_count);
   // for (int row = 0; row < lidarcam_inf_imager_height(lidarcam_diag_context); row++)
   // {
   //    int row_offset = row * lidarcam_inf_imager_width(lidarcam_diag_context);
   //    for (int col = 0; col < lidarcam_inf_imager_width(lidarcam_diag_context); col++)
   //       printf("%06.2f ", return_buffer[row_offset + col]);
   //    printf("\n");
   // }
}

static void lidarcam_run_callbk_framedata(vl53l1x_context ctx, vl53l1x_callback_id callbk_id, vl53l1x_framedata_buffer return_buffer)
{
   lidarcam_context lidarcam = &context_list[callbk_id];

   if (lidarcam->cb_framedata)
      lidarcam->cb_framedata(lidarcam, lidarcam->observer_id, return_buffer);
}
