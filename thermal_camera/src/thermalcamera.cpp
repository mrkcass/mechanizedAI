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
   amg8833_context device;
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
static int thermcam_device_id_map[] = {0, AMG8833_DEVICE_1};

static int contextlist_used;
static THERMCAM_CONTEXT context_list[THERMCAM_MAX_CONTEXTS];

static bool thermcam_can_open(thermcam_id id);


//static AHRS_EULER_CALLBACK outputdata_euler_callbk;
//static AHRS_QUATERNION_CALLBACK outputdata_quaternion_callbk;
//static AHRS_MAGNETOMETER_CALLBACK outputdata_magnetometer_callbk;


//static void ahrs_test_euler_callback(thermcam_context camt, float heading, float pitch, float roll);
//static void ahrs_test_quaternion_callback(thermcam_context camt, float w, float x, float y, float z);
//static void ahrs_test_magnetometer_callback(thermcam_context camt, int mag_x, int mag_y, int mag_z);

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
   ctx->device = amg8833_open(thermcam_device_id_map[id]);

   contextlist_used++;

   return ctx;
}

void thermcam_close(thermcam_context camt)
{
   amg8833_close(camt->device);
}

void thermcam_info(thermcam_context camt)
{
   printf("THERMCAM device info: %s -------------\n", thermcam_names[thermcam_context_to_id(camt)]);
   amg8833_info(camt->device);
}

int thermcam_context_to_id(thermcam_context camt)
{
   return camt->camera_id;
}

int thermcam_diagnostics(thermcam_context camt, thermcam_diagnosticid output_format)
{
   return 0;
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



