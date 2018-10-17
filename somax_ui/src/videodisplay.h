#ifndef __videodisplay_h__
#define __videodisplay_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: videodisplay.h
// DESCRIPTION: Somax video display interface which encapsulates functionality
//              provided by video diplays.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include "pixelbuffer.h"

#define VIDDISP_DISPLAYID_OLED1500RGB565   1
#define VIDDISP_DISPLAYID_OLED0096MONO     2
#define VIDDISP_DISPLAYID_REMOTE_ANDROID   3
#define VIDDISP_NUM_DISPLAYID              3

#define VIDISP_MAX_CONTEXTS 16

struct VIDDISP_CONTEXT;
typedef struct VIDDISP_CONTEXT *viddisp_context;
typedef int viddisp_displayid;
//supplied by device display driver
typedef int viddisp_deviceid;
typedef smx_byte* viddisp_frame_buffer;

viddisp_context viddisp_ini_open(viddisp_displayid device_id);
void viddisp_ini_close(viddisp_context ctx);

char * viddisp_inf_name(viddisp_context ctx);
int viddisp_inf_width(viddisp_context ctx);
int viddisp_inf_height(viddisp_context ctx);
int viddisp_inf_bytes_per_pixel(viddisp_context ctx);
pixbuf_context viddisp_inf_framebuffer(viddisp_context ctx);

pixbuf_context viddisp_opr_newpixbuf(viddisp_context ctx, int width, int height);
void viddisp_opr_refresh(viddisp_context ctx);

#endif