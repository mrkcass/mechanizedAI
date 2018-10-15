#ifndef __pixelbuffer_h__
#define __pixelbuffer_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: pixelbuffer.h
// DESCRIPTION: Somax pixel buffer which provides storage, conversion and
//              manipulation for 2D pixel images of varying shapes and color
//              depths.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#define PIXBUF_FORMATID_RGB565   1
#define PIXBUF_FORMATID_MONO     2
#define PIXBUF_NUM_FORMATS       2

#define PIXBUF_MAX_CONTEXTS      128

struct PIXBUF_CONTEXT;
typedef struct PIXBUF_CONTEXT *pixbuf_context;
typedef int pixbuf_formatid;



pixbuf_context pixbuf_ini_open(pixbuf_formatid format_id, int width, int height);
void pixbuf_ini_close(pixbuf_context ctx);

void pixbuf_opr_blit(pixbuf_context dest, pixbuf_context src);

#endif