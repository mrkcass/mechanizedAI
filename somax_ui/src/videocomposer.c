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
#include <pthread.h>
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
   vidcomp_update_observer update_observer[VIDCOMP_MAX_OBSERVERS];
   int update_observer_count;
   vidcomp_render_observer render_observer[VIDCOMP_MAX_OBSERVERS];
   int render_observer_count;
   pthread_t refresh_threadid;
   smx_ui32 refresh_delay;
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
static void *vidcomp_refresh(void *arg);
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

bool vidcomp_add_update_observer(vidcomp_context ctx, vidcomp_update_observer observer)
{
   if (ctx->update_observer_count >= VIDCOMP_MAX_OBSERVERS)
   {
      somax_log_add(SOMAX_LOG_ERR, "VIDEOCOMPOSER: add update observer. can't add, max observers reached");
      return false;
   }

   ctx->update_observer[ctx->update_observer_count] = observer;
   ctx->update_observer_count++;

   return true;
}

bool vidcomp_add_render_observer(vidcomp_context ctx, vidcomp_render_observer observer)
{
   if (ctx->render_observer_count >= VIDCOMP_MAX_OBSERVERS)
   {
      somax_log_add(SOMAX_LOG_ERR, "VIDEOCOMPOSER: add render observer. can't add, max observers reached");
      return false;
   }

   ctx->render_observer[ctx->render_observer_count] = observer;
   ctx->render_observer_count++;

   return true;
}

viddisp_context vidcomp_inf_videodisplay(vidcomp_context ctx)
{
   return ctx->video_display;
}

bool vidcomp_opr_run(vidcomp_context ctx, int frame_rate)
{
   if (ctx->refresh_threadid)
   {
      somax_log_add(SOMAX_LOG_WARN, "VIDEOCOMPOSER: run. refresh thread already running");
      return false;
   }
   ctx->refresh_delay = (1000 / frame_rate) * U_MILLISECOND;
   int error = pthread_create(&ctx->refresh_threadid, NULL, &vidcomp_refresh, ctx);
   if (error)
   {
      somax_log_add(SOMAX_LOG_ERR, "VIDEOCOMPOSER: run. refresh thread could not be created");
      return true;
   }
   return false;
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

static void *vidcomp_refresh(void *arg)
{
   vidcomp_context composer = (vidcomp_context)arg;
   pixbuf_context frame_buffer = viddisp_inf_framebuffer(composer->video_display);
   while (1)
   {
      somax_sleep(composer->refresh_delay);
      for (int i = 0; i < VIDCOMP_MAX_OBSERVERS; i++)
      {
         if (!composer->update_observer[i])
            continue;
         composer->update_observer[i](composer);
      }
      for (int i = 0; i < VIDCOMP_MAX_OBSERVERS; i++)
      {
         if (!composer->render_observer[i])
            continue;
         composer->render_observer[i](composer, frame_buffer);
      }
      viddisp_opr_refresh(composer->video_display);
   }
}