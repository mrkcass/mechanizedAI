//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: inputmixer.c
// DESCRIPTION: Somax input mixer provides a means to configure and query input
//              devices.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include "somax.h"
#include "inputmixer.h"
#include "inputsource.h"

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
struct INPUTMIXER_CONTEXT
{
   int context_slotid;
   inputsrc_context sources[INPUTMIX_NUM_SOURCEID];
   int num_sources;
   inputmix_applicationid applications[INPUTMIX_MAX_APPLICATIONS];
   int num_applications;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static INPUTMIXER_CONTEXT mixer;
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool inputmix_can_open();

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
inputmix_applicationid inputmix_ini_open()
{
   if (!inputmix_can_open())
      return 0;

   mixer.num_applications++;

   return mixer.num_applications;
}

void inputmix_ini_close(inputmix_applicationid appid)
{
   mixer.num_applications--;
}

bool inputmix_cfg_source(inputmix_applicationid appid, inputmix_sourceid source_id, bool enable)
{
   bool error = false;
   return error;
}

bool inputmix_cfg_event(inputmix_applicationid appid, inputmix_sourceid source_id, inputevt_eventid event_id, bool enable)
{
   bool error = false;
   return error;
}

bool inputmix_cfg_event_observer(inputmix_applicationid appid, inputmix_event_observer observer)
{
   bool error = false;
   return error;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool inputmix_can_open()
{
   return true;
}