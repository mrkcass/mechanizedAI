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

//SUIMENU is short for:
//  Somax User Interface Menu

#include "videocomposer.h"

#define SUIMENU_MENU_MAX_ITEMS 8

#define SUIMENU_MENUID_NULL         0
#define SUIMENU_MENUID_APPLICATION  1

#define SUIMENU_STATEID_START         1000
#define SUIMENU_STATEID_STOP          1001
#define SUIMENU_STATEID_MENU_ON       1002
#define SUIMENU_STATEID_MENU_OFF      1003
#define SUIMENU_STATEID_MENU_BACK     1004


#define SUIMENU_ITEM_MAX_CHARS 16

struct SUIMENU_CONTEXT;
typedef struct SUIMENU_CONTEXT *suimenu_context;
typedef int suimenu_menuid;
typedef int suimenu_itemid;
typedef int suimenu_stateid;

typedef void (*suimenu_item_observer)(suimenu_menuid menu_id, suimenu_itemid item_id);
typedef void (*suimenu_state_observer)(suimenu_menuid menu_id, suimenu_stateid);

suimenu_context suimenu_ini_open(vidcomp_context video_composer);
void suimenu_ini_close(suimenu_context ctx);
suimenu_menuid suimenu_cfg_addmenu(suimenu_context menu, const char *text, suimenu_state_observer state_observer);
suimenu_itemid suimenu_cfg_additem(suimenu_context menu, suimenu_menuid menu_id, const char * text,
                                   suimenu_menuid submenu_id, suimenu_item_observer item_observer);
void suimenu_cfg_itemtext(suimenu_context menu_ctx, suimenu_menuid menu_id, suimenu_itemid item_id,  const char *text);

bool suimenu_inf_showing(suimenu_context menu);
void suimenu_opr_show(suimenu_context menu, bool show);

#endif