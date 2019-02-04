//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: app_gimbalhold.c
// DESCRIPTION: Hold selected gimbal heading, pitch, and roll independent from frame
//              movement.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <pthread.h>

#include "opencv2/core/core_c.h"
#include "opencv2/core/types_c.h"
#include "opencv2/core/types_c.h"
#include "opencv2/imgproc/imgproc_c.h"

#include "somax.h"
#include "videocomposer.h"
#include "somaxui_menu.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
vidcomp_context gimbalhold_vidcomposer;
static void gimbalhold_update_observer(vidcomp_context ctx);
static void gimbalhold_render_observer(vidcomp_context ctx, pixbuf_context frame_buffer);
static bool gimbalhold_server_run();
static void gimbalhold_init_menu(suimenu_context sui_menu);
static void gimbalhold_menu_state_observer(suimenu_menuid menu_id, suimenu_itemid item_id);
static void gimbalhold_menu_item_observer(suimenu_menuid menu_id, suimenu_itemid item_id);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA STRUCTURES
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

static bool gimbalhold_showing;

static const char gimbalhold_menu_name[] = "gimbal hold";
static const char gimbalhold_menuitem_target[] = "target";
static const char gimbalhold_menuitem_recvideo[] = "rec video";
static suimenu_menuid gimbalhold_menuid_target;
static suimenu_menuid gimbalhold_menuid_recvideo;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int gimbalhold_init(int argc, char *argv[], suimenu_context main_menu, vidcomp_context video_composer)
{
   int return_code = 0;

   gimbalhold_vidcomposer = video_composer;
   vidcomp_add_update_observer(gimbalhold_vidcomposer, gimbalhold_update_observer);
   vidcomp_add_render_observer(gimbalhold_vidcomposer, gimbalhold_render_observer);

   gimbalhold_init_menu(main_menu);

   if (!return_code && !gimbalhold_server_run())
      return_code = 1;

   return return_code;
}

void gimbalhold_show(bool show)
{
   gimbalhold_showing = show;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void gimbalhold_init_menu(suimenu_context menu_ctx)
{
   suimenu_menuid menuid;

   //application menu - will be displayed when the select button is pressed.
   menuid = suimenu_cfg_addmenu(menu_ctx, gimbalhold_menu_name, gimbalhold_menu_state_observer);
   gimbalhold_menuid_target = suimenu_cfg_additem(menu_ctx, menuid, gimbalhold_menuitem_target, SUIMENU_MENUID_NULL, gimbalhold_menu_item_observer);
   gimbalhold_menuid_recvideo = suimenu_cfg_additem(menu_ctx, menuid, gimbalhold_menuitem_recvideo, SUIMENU_MENUID_NULL, gimbalhold_menu_item_observer);

   //add thermal view to the application menu
   suimenu_cfg_additem(menu_ctx, SUIMENU_MENUID_APPLICATION, gimbalhold_menu_name, menuid, gimbalhold_menu_item_observer);
}

static void gimbalhold_menu_state_observer(suimenu_menuid menu_id, suimenu_itemid item_id)
{
   //start the camera and send data to the oled
   if (menu_id == SUIMENU_MENUID_APPLICATION && item_id == SUIMENU_STATEID_START)
   {
   }

   //stop the camera and display update
   if (menu_id == SUIMENU_MENUID_APPLICATION && item_id == SUIMENU_STATEID_STOP)
   {
   }
}

static void gimbalhold_menu_item_observer(suimenu_menuid menu_id, suimenu_itemid item_id)
{
}

void gimbalhold_update_observer(vidcomp_context ctx)
{
   if (!gimbalhold_showing)
      return;
}

void gimbalhold_render_observer(vidcomp_context ctx, pixbuf_context frame_buffer)
{
   if (!gimbalhold_showing)
      return;
}

static void *gimbalhold_server(void *arg)
{
   return 0;
}

static bool gimbalhold_server_run()
{
   pthread_t thread_id;
   int error = pthread_create(&thread_id, NULL, &gimbalhold_server, NULL);
   if (error)
   {
      somax_log_add(SOMAX_LOG_ERR, "gimbalhold SERVER: run. thread could not be created");
      return false;
   }
   return true;
}