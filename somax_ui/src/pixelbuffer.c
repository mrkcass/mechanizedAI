//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: pixelbuffer.c
// DESCRIPTION: Somax pixel buffer which provides storage, conversion and
//              manipulation for 2D pixel images of varying shapes and color
//              depths.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include "somax.h"
#include "pixelbuffer.h"

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
struct PIXBUF_CONTEXT
{
   pixbuf_formatid format_id;
   int width, height;
   int alpha;
   smx_byte * pixels;
   int context_slot;
};

struct PIXBUF_PROPS
{
   pixbuf_formatid format_id;
   int bytes_per_pixel;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static PIXBUF_PROPS pixbuf_properites[PIXBUF_NUM_FORMATS+1] =
{
   {
      0,
      0,
   },
   {
      PIXBUF_FORMATID_RGB565,
      2,
   },
   {
      PIXBUF_FORMATID_MONO,
      1,
   }
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool pixbuf_can_open(pixbuf_formatid format_id, int width, int height);
static int contextlist_used;
static pixbuf_context context_list[PIXBUF_MAX_CONTEXTS];

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
pixbuf_context pixbuf_ini_open(pixbuf_formatid format_id, int width, int height)
{
   if (!pixbuf_can_open(format_id, width, height))
   {
      somax_log_add(SOMAX_LOG_ERR, "PIXELBUFFER. open. can't open: format(%d) %dx%d", format_id, width, height);
      return 0;
   }

   pixbuf_context ctx = (pixbuf_context)somax_malloc(sizeof(struct PIXBUF_CONTEXT));

   ctx->format_id = format_id;
   ctx->width = width;
   ctx->height = height;
   ctx->pixels = (smx_byte*)somax_malloc(pixbuf_properites[format_id].bytes_per_pixel * width * height);

   context_list[contextlist_used] = ctx;
   contextlist_used++;

   return ctx;
}

void pixbuf_ini_close(pixbuf_context pixbuf)
{
   context_list[pixbuf->context_slot] = 0;
   contextlist_used--;
   somax_free(pixbuf->pixels);
   somax_free(pixbuf);
}

void pixbuf_opr_blit(pixbuf_context dest, int dest_x, int dest_y, pixbuf_context src, int src_x, int src_y, int src_w, int src_h)
{

}

smx_byte* pixbuf_inf_pixels(pixbuf_context pixbuf)
{
   return pixbuf->pixels;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool pixbuf_can_open(pixbuf_formatid format_id, int width, int height)
{
   if (format_id <= 0 || format_id > PIXBUF_NUM_FORMATS)
   {
      somax_log_add(SOMAX_LOG_ERR, "PIXELBUFFER. open. format id (%d) out of bounds", format_id);
      return false;
   }

   return true;
}