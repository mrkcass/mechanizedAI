#ifndef __inputsource_h__
#define __inputsource_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: inputsource.h
// DESCRIPTION: Somax input source maps input device specific commends to generic
//              input commands.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include "somax.h"
#include "inputevent.h"

#define INPUTSRC_DEVICEID_JOY3D     1
#define INPUTSRC_DEVICEID_JOY4D     2
#define INPUTSRC_DEVICEID_AHRS      3
#define INPUTSRC_DEVICEID_THERMCAM  4
#define INPUTSRC_DEVICEID_PROXCAM   5
#define INPUTSRC_NUM_DEVICEID       5

#define INPUTSRC_CLASSID_JOYSTICK      1
#define INPUTSRC_CLASSID_MOUSE         2
#define INPUTSRC_CLASSID_STILLCAM      3
#define INPUTSRC_CLASSID_MOTIONCAM     4
#define INPUTSRC_CLASSID_ORIENTATION   5

struct INPUTSRC_CONTEXT;
typedef struct INPUTSRC_CONTEXT *inputsrc_context;
typedef int inputsrc_deviceid;
typedef int inputsrc_classid;

inputsrc_context inputsrc_ini_open(inputsrc_deviceid id);
void inputsrc_ini_close(inputsrc_context ctx);
inputsrc_classid inputsrc_inf_classid(inputsrc_context ctx);
inputevt_eventid inputsrc_inf_events(inputsrc_context ctx);

#endif