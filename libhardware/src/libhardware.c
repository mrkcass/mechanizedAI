//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: i2c_interface.c
// DESCRIPTION: libhardware common initialization and configuration.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "somax.h"
#include "mraa.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool libhardware_init()
{
   mraa_init();
   return true;
}

void libhardware_deinit()
{
   mraa_deinit();
}
