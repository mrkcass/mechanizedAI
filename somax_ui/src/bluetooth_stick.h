#ifndef __bluetooth_stick_h__
#define __bluetooth_stick_h__
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

#define BLUEJOY_MAX_AXIS 4
#define BLUEJOY_AXIS_X       0
#define BLUEJOY_AXIS_Y       1
#define BLUEJOY_AXIS_Z       2
#define BLUEJOY_AXIS_W       3
#define BLUEJOY_BUTTON       4

typedef void (*bluejoy_input_observer)(int axis, char pos);


int bluetoothjoy_connect(bluejoy_input_observer observer, int num_power_levels);
int bluetoothjoy_run();

#endif