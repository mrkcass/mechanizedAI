//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: inputsource.c
// DESCRIPTION: Somax input source maps input device specific commends to generic
//              input commands.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include "somax.h"
#include "inputsource.h"
#include "inputevent.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA STRUCTURES
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct INPUTSRC_CONTEXT
{
   inputsrc_deviceid device_id;
};

struct INPUTSRC_PROPS
{
   inputsrc_deviceid device_id;
   inputsrc_classid class_id;
   inputevt_eventid events;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static INPUTSRC_PROPS inputsrc_properites[INPUTSRC_NUM_DEVICEID + 1] =
{
   {
      0,
      0,
   },
   {
      INPUTSRC_DEVICEID_JOY3D,
      INPUTSRC_CLASSID_JOYSTICK,
      INPUTEVT_EVENTID_UP | INPUTEVT_EVENTID_DOWN | INPUTEVT_EVENTID_LEFT |
            INPUTEVT_EVENTID_PRESSED | INPUTEVT_EVENTID_RELEASED | INPUTEVT_EVENTID_CLICKED,
   },
   {
      INPUTSRC_DEVICEID_JOY4D,
      INPUTSRC_CLASSID_JOYSTICK,
      INPUTEVT_EVENTID_UP | INPUTEVT_EVENTID_DOWN | INPUTEVT_EVENTID_LEFT |
      INPUTEVT_EVENTID_RIGHT | INPUTEVT_EVENTID_PRESSED |
      INPUTEVT_EVENTID_RELEASED | INPUTEVT_EVENTID_CLICKED,
   },
   {
      INPUTSRC_DEVICEID_AHRS,
      INPUTSRC_CLASSID_ORIENTATION,
      INPUTEVT_EVENTID_ARRAYF1D,
   },
   {
      INPUTSRC_DEVICEID_THERMCAM,
      INPUTSRC_CLASSID_MOTIONCAM,
      INPUTEVT_EVENTID_VALUEP | INPUTEVT_EVENTID_ARRAYF2D,
   }
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool inputsrc_can_open(inputsrc_deviceid);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
inputsrc_context inputsrc_ini_open(inputsrc_deviceid device_id)
{
   if (!inputsrc_can_open(device_id))
   {
      somax_log_add(SOMAX_LOG_ERR, "INPUTSRC. open. can't open device id(%d)", device_id);
      return 0;
   }

   inputsrc_context ctx = (inputsrc_context)somax_malloc(sizeof(struct INPUTSRC_CONTEXT));

   ctx->device_id = device_id;

   return ctx;
}

void inputsrc_ini_close(inputsrc_context source)
{
}

inputsrc_classid inputsrc_inf_classid(inputsrc_context ctx)
{
   return inputsrc_properites[ctx->device_id].class_id;
}

inputevt_eventid inputsrc_inf_events(inputsrc_context ctx)
{
   return inputsrc_properites[ctx->device_id].events;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool inputsrc_can_open(inputsrc_deviceid device_id)
{
   if (device_id <= 0 || device_id > INPUTSRC_NUM_DEVICEID)
   {
      somax_log_add(SOMAX_LOG_ERR, "INPUTSRC. open. unkown device id (%d)", device_id);
      return false;
   }

   return true;
}