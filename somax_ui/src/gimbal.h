#ifndef __gimbal_h__
#define __gimbal_h__
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

void gimbal_ini_open();
void gimbal_ini_close();
void gimbal_opr_enable_motors(bool enable);
void gimbal_opr_enable_input(bool enable);
void gimbal_inputmixer_injector(input_event event);

#endif