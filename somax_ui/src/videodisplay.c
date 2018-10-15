//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: videodisplay.c
// FILE: videodisplay.h
// DESCRIPTION: Somax video display interface which encapsulates functionality
//              provided by video diplays.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "somax.h"
#include "videodisplay.h"
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
struct VIDDISP_CONTEXT
{
   int context_slot;
   int device_id;
   pixbuf_context frame_buffer;
};

struct VIDDISP_PROPS
{
   char name[32];
   viddisp_deviceid device_id;
   int width;
   int height;
   pixbuf_formatid pixel_format;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static int contextlist_used;
static struct VIDDISP_PROPS display_properties[VIDDISP_NUM_DISPLAYS+1] =
{
   {
      "NULL",
      0,
      0,
      0,
      0,
   },
   {
      "FRAME-PRIMARY",
      VIDDISP_DEVICEID_OLED1500RGB565,
      128,
      128,
      PIXBUF_FORMATID_RGB565,
   },
   {
      "FRAME-SECONDARY",
      VIDDISP_DEVICEID_OLED1500RGB565,
      128,
      64,
      PIXBUF_FORMATID_MONO,
   },
   {
      "ANDROID-REMOTE",
      VIDDISP_DEVICEID_REMOTE_ANDROID,
      0,
      0,
      PIXBUF_FORMATID_MONO,
   },
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool viddisp_can_open(viddisp_deviceid id);
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
viddisp_context viddisp_ini_open(viddisp_deviceid device_id)
{
   if (!viddisp_can_open(device_id))
   {
      somax_log_add(SOMAX_LOG_ERR, "VIDEODISPLAY. open. unknown devcice id: %d", device_id);
      return 0;
   }

   contextlist_used++;

   return 0;
}

void viddisp_ini_close(viddisp_context ctx)
{

}

char *viddisp_inf_name(viddisp_context ctx)
{
   return display_properties[ctx->device_id].name;
}

int viddisp_inf_width(viddisp_context ctx)
{
   return display_properties[ctx->device_id].width;
}

int viddisp_inf_height(viddisp_context ctx)
{
   return display_properties[ctx->device_id].height;
}

int viddisp_inf_bytes_per_pixel(viddisp_context ctx)
{
   return display_properties[ctx->device_id].pixel_format;
}

pixbuf_context viddisp_opr_newpixbuf(viddisp_context ctx, int width, int height)
{
   return pixbuf_ini_open(display_properties[ctx->device_id].pixel_format, width, height);
}

void viddisp_opr_blit(viddisp_context ctx, pixbuf_context pbuf, int x, int y)
{

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool viddisp_can_open(viddisp_deviceid id)
{
   if (id <= 0 || id > VIDDISP_NUM_DISPLAYS)
   {
      somax_log_add(SOMAX_LOG_ERR, "VIDEODISPLAY. open id (%d) out of bounds", id);
      return false;
   }

   return true;
}