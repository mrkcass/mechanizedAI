#ifndef __inputevent_h__
#define __inputevent_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: inputevent.h
// DESCRIPTION: Somax input provides a common interface to various input events.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include "somax.h"

#define INPUTEVT_EVENTID_UNKOWN       0
#define INPUTEVT_EVENTID_PRESSED      1
#define INPUTEVT_EVENTID_RELEASED     2
#define INPUTEVT_EVENTID_CLICKED      3
#define INPUTEVT_EVENTID_XCLICKED     4
#define INPUTEVT_EVENTID_UP           5
#define INPUTEVT_EVENTID_DOWN         6
#define INPUTEVT_EVENTID_VALUEI       7
#define INPUTEVT_EVENTID_VALUEF       8
#define INPUTEVT_EVENTID_VALUEP       9
#define INPUTEVT_EVENTID_ARRAYI1D     10
#define INPUTEVT_EVENTID_ARRAYI2D     11
#define INPUTEVT_EVENTID_ARRAYF1D     12
#define INPUTEVT_EVENTID_ARRAYF2D     13
#define INPUTEVT_NUM_EVENTID          14

#define INPUTEVT_DTYPEID_UNKNOWN      0
#define INPUTEVT_DTYPEID_INT          1
#define INPUTEVT_DTYPEID_FLOAT        2
#define INPUTEVT_DTYPEID_INTARRAY     3
#define INPUTEVT_DTYPEID_FLOATARRAY   4

//pressed event
#define INPUTEVT_FIELDID_PRESSED_BTN      0
//released event
#define INPUTEVT_FIELDID_RELEASED_BTN     0
//up event
#define INPUTEVT_FIELDID_UP_AXIS          0
#define INPUTEVT_FIELDID_UP_SPEED         1
//down event
#define INPUTEVT_FIELDID_DOWN_AXIS        0
#define INPUTEVT_FIELDID_DOWN_SPEED       1
//clicked event
#define INPUTEVT_FIELDID_CLICKED_BTN      0
//clicked event
#define INPUTEVT_FIELDID_XCLICKED_BTN           0
#define INPUTEVT_FIELDID_XCLICKED_NUMCLICKS     1
//up event
#define INPUTEVT_FIELDID_UP_AXIS          0
#define INPUTEVT_FIELDID_UP_POWER         1
//up event
#define INPUTEVT_FIELDID_DOWN_AXIS        0
#define INPUTEVT_FIELDID_DOWN_POWER       1


struct INPUT_EVENT;
typedef struct INPUT_EVENT *input_event;
typedef int inputevt_eventid;
typedef int inputevt_deviceid;
typedef int inputevt_fieldid;
typedef int inputevt_device_fieldid;
typedef int inputevt_dtypeid;

input_event inputevt_ini_new(inputevt_eventid event_id, inputevt_deviceid device_id);
void inputevt_ini_dispose(input_event evt);

inputevt_eventid inputevt_inf_id(input_event event);
inputevt_dtypeid inputevt_inf_field_datatype(input_event event, inputevt_fieldid field_id);
float inputevt_inf_field_f(input_event event, inputevt_fieldid field_id);
int inputevt_inf_field_i(input_event event, inputevt_fieldid field_id);

void inputevt_cfg_field_f(input_event event, inputevt_fieldid field_id, float value);
void inputevt_cfg_field_i(input_event event, inputevt_fieldid field_id, int value);

#endif