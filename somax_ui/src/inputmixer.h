#ifndef __inputmixer_h__
#define __inputmixer_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: inputmixer.h
// DESCRIPTION: Somax input mixer is a many to one input source mixer.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include "somax.h"
#include "inputevent.h"

#define INPUTMIX_SOURCEID_JOY3D        1
#define INPUTMIX_SOURCEID_JOY4D        2
#define INPUTMIX_SOURCEID_AHRS         3
#define INPUTMIX_SOURCEID_THERMCAM     4
#define INPUTMIX_NUM_SOURCEID          4

#define INPUTMIX_MAX_APPLICATIONS      8

struct INPUTMIX_CONTEXT;
typedef struct INPUTMIX_CONTEXT *inputmix_context;
typedef int inputmix_sourceid;
typedef int inputmix_applicationid;

typedef void (*inputmix_event_observer)(input_event event);

inputmix_applicationid inputmixer_ini_open();
void inputmix_ini_close(inputmix_applicationid appid);

bool inputmix_cfg_source(inputmix_applicationid app_id, inputmix_sourceid source_id, bool enable);
bool inputmix_cfg_event(inputmix_applicationid app_id, inputmix_sourceid source_id, inputevt_eventid event_id, bool enable);
bool inputmix_cfg_event_observer(inputmix_applicationid app_id, inputmix_event_observer observer);

#endif