//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: videocomposer.c
// DESCRIPTION: Somax video composer synchronizes multiple inputs to a single
//              video display.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "videocomposer.h"
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
struct VIDCOMP_CONTEXT
{
   int context_slot;
   int display_id;
   viddisp_context video_display;
   vidcomp_update_observer update_observer;
   vidcomp_render_observer render_observer;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static int contextlist_used;
static struct VIDCOMP_CONTEXT context_list[VIDCOMP_MAX_CONTEXTS];
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool vidcomp_can_open(vidcomp_displayid id);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
vidcomp_context vidcomp_ini_open(vidcomp_displayid displayid)
{
   if (!vidcomp_can_open(displayid))
   {
      somax_log_add(SOMAX_LOG_ERR, "VIDEOCOMPOSER. open. unknown display id: %d", displayid);
      return 0;
   }

   vidcomp_context ctx = &context_list[contextlist_used];
   ctx->display_id = displayid;
   ctx->context_slot = contextlist_used;
   ctx->video_display = viddisp_ini_open(displayid);

   contextlist_used++;

   return ctx;
}

void vidcomp_ini_close(vidcomp_context ctx)
{
}

void vidcomp_add_update_observer(vidcomp_context ctx, vidcomp_update_observer observer)
{
}

void vidcomp_add_render_observer(vidcomp_context ctx, vidcomp_render_observer observer)
{
}

viddisp_context vidcomp_inf_videodisplay(vidcomp_context ctx)
{
   return ctx->video_display;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool vidcomp_can_open(vidcomp_displayid id)
{
   if (id <= 0 || id > VIDCOMP_NUM_DISPLAYS)
   {
      somax_log_add(SOMAX_LOG_ERR, "VIDEOCOMPOSER. open id (%d) out of bounds", id);
      return false;
   }
   if (contextlist_used >= VIDCOMP_MAX_CONTEXTS)
   {
      somax_log_add(SOMAX_LOG_ERR, "VIDEOCOMPOSER. opening context (%d). max contexts already open", id);
      return false;
   }

   return true;
}