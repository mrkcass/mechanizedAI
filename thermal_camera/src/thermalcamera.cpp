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
   THERMCAM_FRAMEDATA_BUFFER frame_buffer;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
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

static bool thermcam_can_open(thermcam_id id);
static int thermcam_diagnostics_toconsole(thermcam_context camt);
static void thermcam_diag_toconsole_callbk(amg8833_context ctx, AMG8833_FRAMEDATA_BUFFER return_buffer);

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

int thermcam_diagnostics(thermcam_context camt, thermcam_diagnosticid output_format)
{
   return thermcam_diagnostics_toconsole(camt);
}

int thermcam_imager_width(thermcam_context camt)
{
   return AMG8833_ARRAY_WIDTH;
}

int thermcam_imager_height(thermcam_context camt)
{
   return AMG8833_ARRAY_HEIGHT;
}

float thermcam_imager_resolution_celsius(thermcam_context camt)
{
   return AMG8833_PIXEL_TEMP_RESOLUTION_CELSIUS;
}

float thermcam_imager_range_min_celsius(thermcam_context camt)
{
   return AMG8833_PIXEL_TEMP_MIN_CELSIUS;
}

float thermcam_imager_range_max_celsius(thermcam_context camt)
{
   return AMG8833_PIXEL_TEMP_MAX_CELSIUS;
}

int thermcam_imager_range_max_milimeters(thermcam_context camt)
{
   return AMG8833_MAX_RANGE_MILLIMETERS;
}

float thermcam_imager_field_of_view_degrees(thermcam_context camt)
{
   return AMG8833_FIELD_OF_VIEW_DEGREES;
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

static int thermcam_diagnostics_toconsole(thermcam_context camt)
{
   thermcam_diag_context = camt;
   amg8833_output_callbk_framedata(camt->amg8833, thermcam_diag_toconsole_callbk, camt->frame_buffer);

   amg8833_run(camt->amg8833, AMG8833_NUMFRAMES_ALL);

   return 0;
}

static void thermcam_diag_toconsole_callbk(amg8833_context ctx, AMG8833_FRAMEDATA_BUFFER return_buffer)
{
   static unsigned long frame_count;

   frame_count++;

   if (frame_count % 20 && frame_count != 1)
      return;

   printf("FRAME %04lu --------------------------------------------------\n", frame_count);
   for (int row = 0; row < thermcam_imager_height(thermcam_diag_context); row++)
   {
      int row_offset = row * thermcam_imager_width(thermcam_diag_context);
      for (int col = 0; col < thermcam_imager_width(thermcam_diag_context); col++)
         printf("%06.2f ", return_buffer[row_offset + col]);
      printf("\n");
   }
}
