//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: thermalcamera.c
// DESCRIPTION: Somax thermal imaging interface.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <stdio.h>
#include <math.h>

#include "somax.h"
#include "thermalcamera.h"
#include "amg8833.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define THERMCAM_MAX_CONTEXTS 16

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA STRUCTURES
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct THERMCAM_CONTEXT
{
   int context_slot;
   thermcam_id camera_id;
   amg8833_context amg8833;
   thermcam_framedata_buffer * frame_buffer;
   thermcam_framedata_callback cb_framedata;
   thermcam_framedata_devicetemp_callback cb_framedata_devtemp;
   thermcam_observer_id observer_id;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//todo: this can be extended to map other things to devices other than just
//names. for example open,close and info functions functions .
static const char thermcam_names[][32] =
    {"THERMCAM-NULL", "THERMCAM-GIMBAL[AMG8833]"};
static int thermcam_device_id_map[] = {0, AMG8833_DEVICEID_1};

static int contextlist_used;
static THERMCAM_CONTEXT context_list[THERMCAM_MAX_CONTEXTS];
static thermcam_context thermcam_diag_context;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool thermcam_can_open(thermcam_id id);

static int thermcam_diag_framedata_toconsole(thermcam_context camt);
static void thermcam_diag_toconsole_framedata_callbk(amg8833_context ctx, amg8833_callback_id callbk_id, amg8833_framedata_buffer return_buffer);
static int thermcam_diag_framedata_devtemp_toconsole(thermcam_context camt);
static void thermcam_diag_toconsole_framedata_devtemp_callbk(amg8833_context ctx, amg8833_callback_id callbk_id, amg8833_framedata_buffer return_buffer);

static void thermcam_run_callbk_framedata(amg8833_context ctx, amg8833_callback_id callbk_id, amg8833_framedata_buffer return_buffer);
static void thermcam_run_callbk_framedata_devtemp(amg8833_context ctx, amg8833_callback_id callbk_id, amg8833_framedata_buffer return_buffer);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
thermcam_context thermcam_open(thermcam_id id)
{
   if (!thermcam_can_open(id))
      return NULL;

   thermcam_context ctx = &context_list[contextlist_used];
   ctx->context_slot = contextlist_used;
   ctx->camera_id = id;
   ctx->amg8833 = amg8833_open(thermcam_device_id_map[id]);

   contextlist_used++;

   return ctx;
}

void thermcam_close(thermcam_context camt)
{
   amg8833_close(camt->amg8833);
}

void thermcam_info(thermcam_context camt)
{
   printf("THERMCAM device info: %s -------------\n", thermcam_names[thermcam_context_to_id(camt)]);
   amg8833_info(camt->amg8833);
}

int thermcam_context_to_id(thermcam_context camt)
{
   return camt->camera_id;
}

int thermcam_diagnostics(thermcam_context camt, thermcam_diagnosticid diag_id)
{
   if (diag_id == THERMCAM_DIAGNOSTICID_FRAMEDATA_CONSOLE_OUTPUT)
      return thermcam_diag_framedata_toconsole(camt);
   else
      return thermcam_diag_framedata_devtemp_toconsole(camt);
}

void thermcam_cfg_output_units(thermcam_context ctx, unsigned char output_units)
{
   amg8833_cfg_output_units(ctx->amg8833, output_units);
}

int thermcam_inf_imager_width(thermcam_context camt)
{
   return AMG8833_ARRAY_WIDTH;
}

int thermcam_inf_imager_height(thermcam_context camt)
{
   return AMG8833_ARRAY_HEIGHT;
}

float thermcam_inf_imager_resolution_celsius(thermcam_context camt)
{
   return AMG8833_PIXEL_TEMP_RESOLUTION_CELSIUS;
}

float thermcam_inf_imager_range_min_celsius(thermcam_context camt)
{
   return AMG8833_PIXEL_TEMP_MIN_CELSIUS;
}

float thermcam_inf_imager_range_max_celsius(thermcam_context camt)
{
   return AMG8833_PIXEL_TEMP_MAX_CELSIUS;
}

int thermcam_inf_imager_range_max_milimeters(thermcam_context camt)
{
   return AMG8833_MAX_RANGE_MILLIMETERS;
}

float thermcam_inf_imager_field_of_view_degrees(thermcam_context camt)
{
   return AMG8833_FIELD_OF_VIEW_DEGREES;
}

//thermcam_cfg_observer_framedata
//configure the output observer callback function for thermal camera pixel data
//PARAM: camt - thermal camera context.
//PARAM: observer_id -
void thermcam_cfg_observer_framedata(thermcam_context camt, thermcam_observer_id observer_id, thermcam_framedata_callback observer, thermcam_framedata_buffer * observer_buffer)
{
   camt->cb_framedata = observer;
   camt->frame_buffer = observer_buffer;
   camt->observer_id = observer_id;
}

void thermcam_cfg_observer_framedata_devicetemp(thermcam_context camt, thermcam_observer_id observer_id, thermcam_framedata_devicetemp_callback observer, thermcam_framedata_buffer * observer_buffer)
{
   camt->cb_framedata_devtemp = observer;
   camt->frame_buffer = observer_buffer;
   camt->observer_id = observer_id;
}

// blocking call that runs an observer update loop.
// PARAM: amg8833 - AMG8833 context
// PARAM: num_frames - number of frames to operate over
// RETURN: 1 if stopped before num_frames were returned. 0 otherwise.
int thermcam_run(thermcam_context camt, int num_frames)
{
   if (camt->cb_framedata)
      amg8833_cfg_output_callbk_framedata(camt->amg8833, camt->context_slot, thermcam_run_callbk_framedata, *camt->frame_buffer);
   else if (camt->cb_framedata_devtemp)
      amg8833_cfg_output_callbk_framedata(camt->amg8833, camt->context_slot, thermcam_run_callbk_framedata_devtemp, *camt->frame_buffer);
   return amg8833_run(camt->amg8833, num_frames);
}

void thermcam_stop(thermcam_context camt)
{
   camt->cb_framedata = NULL;
   camt->cb_framedata_devtemp = NULL;
   camt->frame_buffer = NULL;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool thermcam_can_open(thermcam_id id)
{
   if (id <= 0 || id > THERMCAM_NUM_CAMERAS)
   {
      somax_log_add(SOMAX_LOG_ERR, "THERMCAM. open id (%d) out of bounds", id);
      return false;
   }
   if (contextlist_used >= THERMCAM_MAX_CONTEXTS)
   {
      somax_log_add(SOMAX_LOG_ERR, "THERMCAM. opening context (%d). max contexts already open", id);
      return false;
   }

   return true;
}

static int thermcam_diag_framedata_toconsole(thermcam_context camt)
{
   thermcam_diag_context = camt;
   amg8833_cfg_output_callbk_framedata(camt->amg8833, camt->context_slot, thermcam_diag_toconsole_framedata_callbk, (float*)NULL);

   amg8833_run(camt->amg8833, AMG8833_NUMFRAMES_ALL);

   return 0;
}

static int thermcam_diag_framedata_devtemp_toconsole(thermcam_context camt)
{
   thermcam_diag_context = camt;
   amg8833_cfg_output_callbk_framedata(camt->amg8833, camt->context_slot, thermcam_diag_toconsole_framedata_devtemp_callbk, (float *)NULL);

   amg8833_run(camt->amg8833, AMG8833_NUMFRAMES_ALL);

   return 0;
}

static void thermcam_diag_toconsole_framedata_callbk(amg8833_context ctx, amg8833_callback_id callbk_id, amg8833_framedata_buffer return_buffer)
{
   //todo: convert this to call the run callback so that the callback functionality is also tested.
   static unsigned long frame_count;

   frame_count++;

   if (frame_count % 10 && frame_count != 1)
      return;

   printf("FRAME %04lu --------------------------------------------------\n", frame_count);
   for (int row = 0; row < thermcam_inf_imager_height(thermcam_diag_context); row++)
   {
      int row_offset = row * thermcam_inf_imager_width(thermcam_diag_context);
      for (int col = 0; col < thermcam_inf_imager_width(thermcam_diag_context); col++)
         printf("%06.2f ", return_buffer[row_offset + col]);
      printf("\n");
   }
}

static void thermcam_diag_toconsole_framedata_devtemp_callbk(amg8833_context ctx, amg8833_callback_id callbk_id, amg8833_framedata_buffer return_buffer)
{
   //todo: convert this to call the run callback so that the callback functionality is also tested.
   static unsigned long frame_count;

   frame_count++;

   if (frame_count % 10 && frame_count != 1)
      return;

   float dev_temp = amg8833_inf_device_temperature(ctx);

   printf("FRAME %04lu --------------------------------------------------\n", frame_count);
   printf("%06.2f\n", dev_temp);
   for (int row = 0; row < thermcam_inf_imager_height(thermcam_diag_context); row++)
   {
      int row_offset = row * thermcam_inf_imager_width(thermcam_diag_context);
      for (int col = 0; col < thermcam_inf_imager_width(thermcam_diag_context); col++)
         printf("%06.2f ", return_buffer[row_offset + col]);
      printf("\n");
   }
}

static void thermcam_run_callbk_framedata(amg8833_context ctx, amg8833_callback_id callbk_id, amg8833_framedata_buffer return_buffer)
{
   thermcam_context camt = &context_list[callbk_id];

   if (camt->cb_framedata)
      camt->cb_framedata(camt, camt->observer_id, return_buffer);
}

static void thermcam_run_callbk_framedata_devtemp(amg8833_context ctx, amg8833_callback_id callbk_id, amg8833_framedata_buffer return_buffer)
{
   thermcam_context camt = &context_list[callbk_id];

   float dev_temp = amg8833_inf_device_temperature(ctx);

   //todo: this is a mutual exclusion problem. need to wrap cb_frame_data with semaphore
   if (camt->cb_framedata_devtemp)
      camt->cb_framedata_devtemp(camt, camt->observer_id, return_buffer, dev_temp);
}
