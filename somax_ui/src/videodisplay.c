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
#include "videodisplay_ssd1351.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool viddisp_can_open(viddisp_displayid id);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA STRUCTURES
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct VIDDISP_CONTEXT
{
   int context_slot;
   int display_id;
};

struct DEVICE_PROPS
{
   char name[32];
   viddisp_deviceid device_id;
   int width;
   int height;
   int context_used;
   pixbuf_formatid pixel_format;
   pixbuf_context frame_buffer;
};

typedef bool (*device_open) (viddisp_deviceid id);
typedef void (*device_close) (viddisp_deviceid id);
typedef void (*device_display_buffer) (viddisp_deviceid id, viddisp_frame_buffer buffer);
struct DEVICE_FUNCS
{
   device_open             ini_open;
   device_close            ini_close;
   device_display_buffer   opr_buffer;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static struct VIDDISP_CONTEXT context_list[VIDISP_MAX_CONTEXTS];
static int contextlist_used;
static struct DEVICE_PROPS device_props[VIDDISP_NUM_DISPLAYID+1] =
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
      SSD1351_DEVICEID_OLED_0,
      SSD1351_WIDTH,
      SSD1351_HEIGHT,
      PIXBUF_FORMATID_RGB565,
      0,
   },
   {
      "FRAME-SECONDARY",
      0,
      128,
      64,
      PIXBUF_FORMATID_MONO,
      0,
   },
   {
      "ANDROID-REMOTE",
      0,
      0,
      0,
      PIXBUF_FORMATID_RGB888,
      0
   },
};

static struct DEVICE_FUNCS device_ops[VIDDISP_NUM_DISPLAYID+1] =
{
   {0,0,0},
   {
      ssd1351_ini_open,
      ssd1351_ini_close,
      ssd1351_opr_display_buffer,
   },
   {0,0,0},
   {0,0,0},
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
viddisp_context viddisp_ini_open(viddisp_displayid display_id)
{
   if (!viddisp_can_open(display_id))
   {
      somax_log_add(SOMAX_LOG_ERR, "VIDEODISPLAY. open. unknown display id: %d", display_id);
      return 0;
   }

   DEVICE_PROPS * vprops = &device_props[display_id];
   DEVICE_FUNCS * vops = &device_ops[display_id];
   if (!vprops->frame_buffer)
   {
      vops->ini_open(vprops->device_id);
      vprops->frame_buffer = pixbuf_ini_open(vprops->pixel_format, vprops->width, vprops->height);
   }

   viddisp_context new_ctx = &context_list[contextlist_used];
   new_ctx->display_id = display_id;
   new_ctx->context_slot = contextlist_used;

   vprops->context_used++;
   contextlist_used++;

   return new_ctx;
}

void viddisp_ini_close(viddisp_context ctx)
{
   if (ctx == 0)
      return;


   DEVICE_PROPS *vprops = &device_props[ctx->display_id];
   if (vprops->context_used)
   {
      vprops->context_used--;
      if (!vprops->context_used)
         pixbuf_ini_close(vprops->frame_buffer);
   }

   ctx->display_id = 0;
   ctx->context_slot = 0;
   contextlist_used--;
}

char *viddisp_inf_name(viddisp_context ctx)
{
   return device_props[ctx->display_id].name;
}

int viddisp_inf_width(viddisp_context ctx)
{
   return device_props[ctx->display_id].width;
}

int viddisp_inf_height(viddisp_context ctx)
{
   return device_props[ctx->display_id].height;
}

int viddisp_inf_bytes_per_pixel(viddisp_context ctx)
{
   return device_props[ctx->display_id].pixel_format;
}

pixbuf_context viddisp_inf_framebuffer(viddisp_context ctx)
{
   return device_props[ctx->display_id].frame_buffer;
}

pixbuf_context viddisp_opr_newpixbuf(viddisp_context ctx, int width, int height)
{
   return pixbuf_ini_open(device_props[ctx->display_id].pixel_format, width, height);
}

void viddisp_opr_refresh(viddisp_context ctx)
{
   DEVICE_PROPS *vprops = &device_props[ctx->display_id];
   DEVICE_FUNCS *vops = &device_ops[ctx->display_id];
   vops->opr_buffer(vprops->device_id, pixbuf_inf_pixels(vprops->frame_buffer));
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool viddisp_can_open(viddisp_deviceid id)
{
   if (id <= 0 || id > VIDDISP_NUM_DISPLAYID)
   {
      somax_log_add(SOMAX_LOG_ERR, "VIDEODISPLAY. open id (%d) out of bounds", id);
      return false;
   }

   return true;
}