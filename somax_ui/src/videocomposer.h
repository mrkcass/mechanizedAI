#ifndef __videocomposer_h__
#define __videocomposer_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: videocomposer.h
// DESCRIPTION: Somax video display interface which allows many inputs to a
//              single video output.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include "somax.h"
#include "videodisplay.h"

#define VIDCOMP_NUM_DISPLAYS               3
#define VIDCOMP_DISPLAYID_FRAME_PRIMARY    1
#define VIDCOMP_DISPLAYID_FRAME_SECONDARY  2
#define VIDCOMP_DISPLAYID_REMOTE_ANDROID   3

#define VIDCOMP_MAX_CONTEXTS 4

struct VIDCOMP_CONTEXT;
typedef struct VIDCOMP_CONTEXT *vidcomp_context;
typedef int vidcomp_displayid;

typedef void (*vidcomp_update_observer)(vidcomp_context ctx);
typedef void (*vidcomp_render_observer)(vidcomp_context ctx);

vidcomp_context vidcomp_ini_open(vidcomp_displayid displayid);
void vidcomp_ini_close(vidcomp_context ctx);

void vidcomp_add_update_observer(vidcomp_context ctx, vidcomp_update_observer observer);
void vidcomp_add_render_observer(vidcomp_context ctx, vidcomp_render_observer observer);

viddisp_context vidcomp_inf_videodisplay(vidcomp_context ctx);

#endif