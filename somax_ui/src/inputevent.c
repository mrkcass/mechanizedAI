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
   int num_data;
   event_data data;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static struct INPUT_EVENT template_null;
static struct INPUT_EVENT template_pressed =
{
   INPUTEVT_EVENTID_PRESSED,
   0,
   1,
   {
      //device_ref, data assigned by device
      {INPUTEVT_FIELDID_PRESSED_BTN, 0, INPUTEVT_DTYPEID_INT, 0},
   },
};
static struct INPUT_EVENT template_released =
{
   INPUTEVT_EVENTID_RELEASED,
   0,
   1,
   {
      //device_ref, data assigned by device
      {INPUTEVT_FIELDID_RELEASED_BTN, 0, INPUTEVT_DTYPEID_INT, 0},
   },
};
static struct INPUT_EVENT template_clicked =
{
   INPUTEVT_EVENTID_CLICKED,
   0,
   1,
   {
      //device_ref, data assigned by device
      {INPUTEVT_FIELDID_CLICKED_BTN, 0, INPUTEVT_DTYPEID_INT, 0},
   },
};
static struct INPUT_EVENT template_xclicked =
{
   INPUTEVT_EVENTID_XCLICKED,
   0,
   2,
   {
      //device_ref, data assigned by device
      {INPUTEVT_FIELDID_XCLICKED_BTN,       0, INPUTEVT_DTYPEID_INT, 0},
      {INPUTEVT_FIELDID_XCLICKED_NUMCLICKS, 0, INPUTEVT_DTYPEID_INT, 0},
   },
};
static struct INPUT_EVENT template_up =
{
   INPUTEVT_EVENTID_UP,
   0,
   2,
   {
      //device_ref, data assigned by device
      {INPUTEVT_FIELDID_UP_AXIS,  0, INPUTEVT_DTYPEID_INT, 0},
      {INPUTEVT_FIELDID_UP_POWER, 0, INPUTEVT_DTYPEID_INT, 0},
   },
};
static struct INPUT_EVENT template_down =
{
   INPUTEVT_EVENTID_DOWN,
   0,
   2,
   {
      //device_ref, data assigned by device
      {INPUTEVT_FIELDID_DOWN_AXIS,  0, INPUTEVT_DTYPEID_INT, 0},
      {INPUTEVT_FIELDID_DOWN_POWER, 0, INPUTEVT_DTYPEID_INT, 0},
   },
};
static INPUT_EVENT inputevt_templates[INPUTEVT_NUM_EVENTID + 1] =
{
   template_null,
   template_pressed,
   template_released,
   template_clicked,
   template_xclicked,
   template_up,
   template_down,
   template_null,
   template_null,
   template_null,
   template_null,
   template_null,
   template_null,
   template_null,
   template_null,
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
   somax_free(evt);
}

void inputevt_cfg_field_f(input_event event, inputevt_fieldid field_id, float value)
{
   int error = 1;
   inputevt_dtypeid error_typeid;

   for (int i=0; i < event->num_data; i++)
   {
      if (event->data[i].field_id == field_id)
      {
         if (event->data[i].datatype_id == INPUTEVT_DTYPEID_FLOAT)
         {
            event->data[i].data.fval = value;
            error = 0;
         }
         else
         {
            error_typeid = event->data[i].datatype_id;
            error = 2;
         }
         break;
      }
   }

   if (error == 1)
      somax_log_add(SOMAX_LOG_ERR, "inputevt_cfg_field_f. unknown id (%d) for event id (%d)", field_id, event->event_id);
   if (error == 2)
      somax_log_add(SOMAX_LOG_ERR, "inputevt_cfg_field_f. wrong type id (%d) for event id (%d[%d])", error_typeid, event->event_id, field_id);
}

void inputevt_cfg_field_i(input_event event, inputevt_fieldid field_id, int value)
{
   int error = 1;
   inputevt_dtypeid error_typeid;

   for (int i = 0; i < event->num_data; i++)
   {
      if (event->data[i].field_id == field_id)
      {
         if (event->data[i].datatype_id == INPUTEVT_DTYPEID_INT)
         {
            event->data[i].data.ival = value;
            error = 0;
         }
         else
         {
            error_typeid = event->data[i].datatype_id;
            error = 2;
         }
         break;
      }
   }

   if (error == 1)
      somax_log_add(SOMAX_LOG_ERR, "inputevt_cfg_field_i. unknown id (%d) for event id (%d)", field_id, event->event_id);
   if (error == 2)
      somax_log_add(SOMAX_LOG_ERR, "inputevt_cfg_field_i. wrong type id (%d) for event id (%d[%d])", error_typeid, event->event_id, field_id);
}

inputevt_eventid inputevt_inf_id(input_event event)
{
   return event->event_id;
}

inputevt_dtypeid inputevt_inf_field_datatype(input_event event, inputevt_fieldid field_id)
{
   for (int i = 0; i < event->num_data; i++)
   {
      if (event->data[i].field_id == field_id)
      {
         return event->data[i].datatype_id;
      }
   }

   somax_log_add(SOMAX_LOG_ERR, "inputevt_inf_field_datatype. unknown field id (%d) for event id (%d)", field_id, event->event_id);

   return INPUTEVT_DTYPEID_UNKNOWN;
}

float inputevt_inf_field_f(input_event event, inputevt_fieldid field_id)
{
   for (int i = 0; i < event->num_data; i++)
   {
      if (event->data[i].field_id == field_id)
      {
         if (event->data[i].datatype_id == INPUTEVT_DTYPEID_FLOAT)
            return event->data[i].data.fval;
         else
         {
            somax_log_add(SOMAX_LOG_ERR, "inputevt_inf_field_f. event type (%d), field id (%d) is not float", event->event_id, field_id);
            return 0.0;
         }
      }
   }

   somax_log_add(SOMAX_LOG_ERR, "inputevt_inf_field_f. event type (%d) has no field id (%d)", event->event_id, field_id);

   return 0.0;
}

int inputevt_inf_field_i(input_event event, inputevt_fieldid field_id)
{
   for (int i = 0; i < event->num_data; i++)
   {
      if (event->data[i].field_id == field_id)
      {
         if (event->data[i].datatype_id == INPUTEVT_DTYPEID_INT)
            return event->data[i].data.ival;
         else
         {
            somax_log_add(SOMAX_LOG_ERR, "inputevt_inf_field_i. event type (%d), field id (%d) is not integer", event->event_id, field_id);
            return 0.0;
         }
      }
   }

   somax_log_add(SOMAX_LOG_ERR, "inputevt_inf_field_i. event type (%d) has no field id (%d)", event->event_id, field_id);

   return 0.0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool inputevt_can_new(inputevt_eventid event_id)
{
   if (event_id == INPUTEVT_EVENTID_PRESSED ||
       event_id == INPUTEVT_EVENTID_RELEASED ||
       event_id == INPUTEVT_EVENTID_CLICKED ||
       event_id == INPUTEVT_EVENTID_XCLICKED ||
       event_id == INPUTEVT_EVENTID_UP ||
       event_id == INPUTEVT_EVENTID_DOWN)
   {
      return true;
   }

   somax_log_add(SOMAX_LOG_ERR, "INPUTEVT. open. unkown event id (%d)", event_id);
   return false;
}
