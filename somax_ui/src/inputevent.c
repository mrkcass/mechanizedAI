//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: inputevent.c
// DESCRIPTION: Somax input event provides storage for input device streamed
//              output.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <string.h>

#include "somax.h"
#include "inputevent.h"
#include "inputsource.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define INPUTEVT_MAX_FIELDS 8
#define INPUTEVT_DTYPE_UNKNOWN      0
#define INPUTEVT_DTYPE_INT          1
#define INPUTEVT_DTYPE_FLOAT        2
#define INPUTEVT_DTYPE_INTARRAY     3
#define INPUTEVT_DTYPE_FLOATARRAY   4
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA STRUCTURES
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

union INPUTEVT_FIELDDATA
{
   int ival;
   float fval;
   int * iarray;
   float *farray;
};

struct INPUTEVT_FIELD
{
   inputevt_fieldid field_id;
   inputevt_device_fieldid device_ref;
   inputevt_dtypeid datatype_id;
   INPUTEVT_FIELDDATA data;
};

typedef INPUTEVT_FIELD event_data[INPUTEVT_MAX_FIELDS];

struct INPUT_EVENT
{
   inputevt_eventid event_id;
   inputevt_deviceid device_id;
   event_data data;
   int num_data;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static INPUT_EVENT inputevt_templates[INPUTEVT_NUM_EVENTID + 1] =
{
   {
      0,
      0,
      {
         {0,0,0},
      },
      0
   },
   {
      INPUTEVT_EVENTID_PRESSED,
      0,
      {
         //device_ref, data assigned by device
         {INPUTEVT_FIELDID_PRESSED_ISPRESSED, 0, INPUTEVT_DTYPE_INT, 0},
      },
      1,
   },
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool inputevt_can_new(inputevt_eventid event_id);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
input_event inputevt_ini_new(inputevt_eventid event_id, inputevt_deviceid device_id)
{
   if (!inputevt_can_new(event_id))
      return 0;

   input_event evt = (input_event)somax_malloc(sizeof(struct INPUT_EVENT));

   memcpy(evt, &inputevt_templates[event_id], sizeof(INPUT_EVENT));
   evt->device_id = device_id;

   return evt;
}

void inputevt_ini_dispose(input_event evt)
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool inputevt_can_new(inputevt_eventid event_id)
{
   if (event_id <= 0 || event_id > INPUTSRC_NUM_DEVICEID)
   {
      somax_log_add(SOMAX_LOG_ERR, "INPUTEVT. open. unkown event id (%d)", event_id);
      return false;
   }

   return true;
}