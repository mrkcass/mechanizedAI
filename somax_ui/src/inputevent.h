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

#define INPUTEVT_EVENTID_UNKOWN       0b00000000000000000000000000000000
#define INPUTEVT_EVENTID_PRESSED      0b00000000000000000000000000000001
#define INPUTEVT_EVENTID_RELEASED     0b00000000000000000000000000000010
#define INPUTEVT_EVENTID_CLICKED      0b00000000000000000000000000000100
#define INPUTEVT_EVENTID_XCLICKED     0b00000000000000000000000000001000
#define INPUTEVT_EVENTID_UP           0b00000000000000000000000000010000
#define INPUTEVT_EVENTID_DOWN         0b00000000000000000000000000100000
#define INPUTEVT_EVENTID_LEFT         0b00000000000000000000000001000000
#define INPUTEVT_EVENTID_RIGHT        0b00000000000000000000000010000000
#define INPUTEVT_EVENTID_VALUEI       0b00000000000000000000000100000000
#define INPUTEVT_EVENTID_VALUEF       0b00000000000000000000001000000000
#define INPUTEVT_EVENTID_VALUEP       0b00000000000000000000010000000000
#define INPUTEVT_EVENTID_ARRAYI1D     0b00000000000000000000100000000000
#define INPUTEVT_EVENTID_ARRAYI2D     0b00000000000000000001000000000000
#define INPUTEVT_EVENTID_ARRAYF1D     0b00000000000000000010000000000000
#define INPUTEVT_EVENTID_ARRAYF2D     0b00000000000000000100000000000000
#define INPUTEVT_NUM_EVENTID          14

//pressed event
#define INPUTEVT_FIELDID_PRESSED_ISPRESSED      0
//released event
#define INPUTEVT_FIELDID_RELEASED_ISRELEASED    0

struct INPUT_EVENT;
typedef struct INPUT_EVENT *input_event;
typedef int inputevt_eventid;
typedef int inputevt_deviceid;
typedef int inputevt_fieldid;
typedef int inputevt_device_fieldid;
typedef int inputevt_dtypeid;

input_event inputevt_ini_new(inputevt_eventid event_id, inputevt_deviceid device_id);
void inputevt_ini_dispose(input_event evt);

#endif