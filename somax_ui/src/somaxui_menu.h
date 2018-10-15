#ifndef __ui_menu_h__
#define __ui_menu_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: somaxui_menu.h
// DESCRIPTION: Somax user interface menu.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#define SUIMENU_MENU_MAX_ITEMS 10

#define SUIMENU_MENUID_NULL   0
#define SUIMENU_MENUID_BACK 1
#define SUIMENU_MENUID_USER1 2

struct SUIMENU_CONTEXT;
typedef struct SUIMENU_CONTEXT *suimenu_context;
typedef int suimenu_menuid;
typedef int suimenu_itemid;

typedef void (*suimenu_observer_selected)(suimenu_context ctx);

suimenu_context suimenu_ini_open(viddisp_context ctx);
void suimenu_ini_close(suimenu_context ctx);

suimenu_id suimenu_cfg_additem(suimenu_context menu, char * text);
void suimenu_cfg_select_observer(suimenu_context menu);
void suimenu_cfg_attach_to_composer(suimenu_context menu, vidcomp_context vidcomp);
void suimenu_cfg_attach_to_mixer(suimenu_context menu);

suimenu_id suimenu_opr_show(suimenu_context menu, bool show);

#endif