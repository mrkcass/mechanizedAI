//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: app_lidarview.c
// DESCRIPTION: Display ranging data and facilitate depth scan imaging.
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
#include "lidarcamera.h"
#include "somaxui_menu.h"
#include "gimbal.h"

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
static vidcomp_context vidcomposer;
static void lidarview_update_observer(vidcomp_context ctx);
static void lidarview_render_observer(vidcomp_context ctx, pixbuf_context frame_buffer);
static bool lidarview_server_run();
static void lidarview_init_menu(suimenu_context sui_menu);
static void lidarvw_menu_state_observer(suimenu_menuid menu_id, suimenu_itemid state_id);
static void lidarvw_menu_item_observer(suimenu_menuid menu_id, suimenu_itemid state_id);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA STRUCTURES
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct LIDARVW_GIMBAL_POS
{
   int x;
   int y;
   int z;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static IplImage * lidarvw_opencv_buff;
static IplImage *lidarvw_opencv_buff_resized_rgb565;
static CvFont lidarvw_opencv_font;

static bool lidarvw_receiving_frame = false;
static bool lidarvw_displaying_frame = false;
static bool lidarvw_show;
static bool lidarvw_initialized;
static bool lidarvw_gimbal_enabled;
//static struct LIDARVW_GIMBAL_POS startpt;
//static struct LIDARVW_GIMBAL_POS endpt;
static float lidarvw_range;

//lidarcam data
lidarcam_framedata_buffer lidarvw_lidarcam_framebuffer;
static lidarcam_context lidarvw_lidarcam;


static suimenu_context lidarvw_menu_ctx;
static char lidarvw_menu_name[] = "lidar view";
static const char lidarvw_menuitem_units[]       = "inches/*millimeters";
static const char lidarvw_menuitem_snapshot[]    = "take snapshot";
static const char lidarvw_menuitem_gimbal_on[]   = "gimbal on";
static const char lidarvw_menuitem_gimbal_off[]  = "gimbal off";

static suimenu_menuid lidarvw_menuid;
static suimenu_itemid lidarvw_itemid_units;
static suimenu_itemid lidarvw_itemid_snapshot;
static suimenu_itemid lidarvw_itemid_gimbal;

static suimenu_menuid lidarvw_menuid_scan;
static suimenu_itemid lidarvw_itemid_scan_endpt;
static suimenu_itemid lidarvw_itemid_scan_startpt;
static suimenu_itemid lidarvw_itemid_scan_scan;
static suimenu_itemid lidarvw_itemid_scan_cancel;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int lidarview_init(int argc, char *argv[], suimenu_context main_menu, vidcomp_context video_composer)
{
   int return_code = 0;

   if (lidarvw_initialized)
      return 0;

   printf("initializing lidarview application\n");

   lidarvw_menu_ctx = main_menu;

   vidcomposer = video_composer;
   vidcomp_add_update_observer(vidcomposer, lidarview_update_observer);
   vidcomp_add_render_observer(vidcomposer, lidarview_render_observer);

   lidarview_init_menu(main_menu);

   if (!return_code && !lidarview_server_run())
      return_code = 1;
   else
      lidarvw_initialized = true;

   return return_code;
}

void lidarview_show(bool show)
{
   lidarvw_show = show;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void lidarview_init_menu(suimenu_context menu_ctx)
{
   lidarvw_menuid_scan = suimenu_cfg_addmenu(menu_ctx, "scan >", lidarvw_menu_state_observer);
   lidarvw_itemid_scan_endpt = suimenu_cfg_additem(menu_ctx, lidarvw_menuid_scan, "endpt", SUIMENU_MENUID_NULL, lidarvw_menu_item_observer);
   lidarvw_itemid_scan_startpt = suimenu_cfg_additem(menu_ctx, lidarvw_menuid_scan, "startpt", SUIMENU_MENUID_NULL, lidarvw_menu_item_observer);
   lidarvw_itemid_scan_scan = suimenu_cfg_additem(menu_ctx, lidarvw_menuid_scan, "scan", SUIMENU_MENUID_NULL, lidarvw_menu_item_observer);
   lidarvw_itemid_scan_cancel  = suimenu_cfg_additem(menu_ctx, lidarvw_menuid_scan, "cancel", SUIMENU_MENUID_NULL, lidarvw_menu_item_observer);

   //application menu - will be displayed when the select button is pressed.
   lidarvw_menuid = suimenu_cfg_addmenu(menu_ctx, lidarvw_menu_name, lidarvw_menu_state_observer);
   lidarvw_itemid_units = suimenu_cfg_additem(menu_ctx, lidarvw_menuid, lidarvw_menuitem_units, SUIMENU_MENUID_NULL, lidarvw_menu_item_observer);
   lidarvw_itemid_snapshot = suimenu_cfg_additem(menu_ctx, lidarvw_menuid, lidarvw_menuitem_snapshot, SUIMENU_MENUID_NULL, lidarvw_menu_item_observer);
   lidarvw_itemid_gimbal = suimenu_cfg_additem(menu_ctx, lidarvw_menuid, lidarvw_menuitem_gimbal_on, SUIMENU_MENUID_NULL, lidarvw_menu_item_observer);
   suimenu_cfg_additem(menu_ctx, lidarvw_menuid, "scan >", lidarvw_menuid_scan, NULL);


   //add thermal view to the application menu
   suimenu_cfg_additem(menu_ctx, SUIMENU_MENUID_APPLICATION, lidarvw_menu_name, lidarvw_menuid, lidarvw_menu_item_observer);
}

static void lidarvw_menu_state_observer(suimenu_menuid menu_id, suimenu_stateid state_id)
{
   //start the camera and send data to the oled
   if (menu_id == SUIMENU_MENUID_APPLICATION && state_id == SUIMENU_STATEID_START)
   {
      printf("starting lidarview application\n");
      lidarview_show(true);
   }

   //stop the camera and display update
   if (menu_id == SUIMENU_MENUID_APPLICATION && state_id == SUIMENU_STATEID_STOP)
   {
      printf("stopping lidarview application\n");
      if (lidarvw_gimbal_enabled)
      {
         gimbal_opr_enable_motors(false);
         gimbal_opr_enable_input(false);
         suimenu_cfg_itemtext(lidarvw_menu_ctx, lidarvw_menuid, lidarvw_itemid_gimbal, lidarvw_menuitem_gimbal_on);
      }
      lidarview_show(false);
   }

   //displaying the menu, save band width by stopping lidarvw_display
   if (menu_id == SUIMENU_MENUID_APPLICATION && state_id == SUIMENU_STATEID_MENU_ON)
   {
      printf("lidarview application - menu on\n");
      lidarview_show(false);
   }
   //displaying the menu, save band width by stopping lidarvw_display
   if (menu_id == SUIMENU_MENUID_APPLICATION && state_id == SUIMENU_STATEID_MENU_OFF)
   {
      printf("lidarview application - menu off\n");
      lidarview_show(true);
   }
}

static void lidarvw_menu_item_observer(suimenu_menuid menu_id, suimenu_itemid item_id)
{
   //enable / disable gimbal
   if (menu_id == lidarvw_menuid && item_id == lidarvw_itemid_gimbal)
   {
      if (!lidarvw_gimbal_enabled)
      {
         printf("lidarview application enable gimbal\n");
         lidarvw_gimbal_enabled = true;
         gimbal_opr_enable_motors(true);
         gimbal_opr_enable_input(true);
         suimenu_cfg_itemtext(lidarvw_menu_ctx, lidarvw_menuid, lidarvw_itemid_gimbal, lidarvw_menuitem_gimbal_off);
      }
      else
      {
         printf("lidarview application disable gimbal\n");
         lidarvw_gimbal_enabled = false;
         gimbal_opr_enable_motors(false);
         gimbal_opr_enable_input(false);
         suimenu_cfg_itemtext(lidarvw_menu_ctx, lidarvw_menuid, lidarvw_itemid_gimbal, lidarvw_menuitem_gimbal_on);
      }
   }

   if (menu_id == lidarvw_menuid_scan)
   {
   }
}

void lidarview_update_draw_crosshair(IplImage *canvas)
{
   CvPoint begin = {63-5, 63};
   CvPoint end = {63+5, 63};
   CvScalar color = {255,255,255};

   cvLine(canvas, begin, end, color, 2, 0);

   begin.x = 63; begin.y = 63-5;
   end.x = 63; end.y = 63+5;
   cvLine(canvas, begin, end, color, 2, 0);
}

void lidarview_update_draw_range(IplImage *canvas, float range)
{
   CvPoint text_loc = {0, 128};
   CvScalar text_color = {255, 255, 255};
   char text_msg[16];

   sprintf(text_msg, "%.1f mm", range);

   cvPutText(canvas, text_msg, text_loc, &lidarvw_opencv_font, text_color);
}

void lidarview_update_observer(vidcomp_context ctx)
{
   if (!lidarvw_show)
      return;

   if (lidarvw_receiving_frame)
      return;

   lidarvw_displaying_frame = true;
   if (!lidarvw_opencv_buff)
   {
      CvSize size;
      size.height = size.width = 128;
      lidarvw_opencv_buff = cvCreateImage(size, IPL_DEPTH_8U, 3);
      lidarvw_opencv_buff_resized_rgb565 = cvCreateImage(size, IPL_DEPTH_8U, 2);
      cvInitFont(&lidarvw_opencv_font, CV_FONT_HERSHEY_PLAIN, 1.0, 1.0, 0, 1, 4);
   }

   CvRect display_rect = {0,0, 128,128};
   CvScalar bkcolor = {10, 0, 10};
   cvRectangleR(lidarvw_opencv_buff, display_rect, bkcolor, CV_FILLED, 8, 0);

   lidarview_update_draw_range(lidarvw_opencv_buff, lidarvw_range);
   lidarview_update_draw_crosshair(lidarvw_opencv_buff);
   cvCvtColor(lidarvw_opencv_buff, lidarvw_opencv_buff_resized_rgb565, CV_RGB2BGR565);
}

void lidarview_render_observer(vidcomp_context ctx, pixbuf_context frame_buffer)
{
   if (!lidarvw_show)
      return;
   memcpy(pixbuf_inf_pixels(frame_buffer), lidarvw_opencv_buff_resized_rgb565->imageData, 128 * 128 * 2);
   lidarvw_displaying_frame = false;
}

static void lidarview_lidarcam_observer(lidarcam_context ctx, lidarcam_observer_id observer_id, lidarcam_framedata_buffer return_buffer)
{
   if (lidarvw_displaying_frame)
      return;

   lidarvw_receiving_frame = true;
   lidarvw_range = return_buffer[0];
   lidarvw_receiving_frame = false;
}

static void* lidarview_server(void *arg)
{
   lidarvw_lidarcam = lidarcam_open(LIDARCAMID_LEFT);

   // lidarvw_cfg_output_units(lidarvw_lidarcam, lidarvw_OUTPUTUNITS_FARENHEIT);
   lidarcam_cfg_observer_framedata(lidarvw_lidarcam, 0, lidarview_lidarcam_observer, &lidarvw_lidarcam_framebuffer);

   lidarcam_run(lidarvw_lidarcam, LIDARCAM_NUMFRAMES_CONTINUOUS);

   return 0;
}

static bool lidarview_server_run()
{
   pthread_t thread_id;
   int error = pthread_create(&thread_id, NULL, &lidarview_server, NULL);
   if (error)
   {
      somax_log_add(SOMAX_LOG_ERR, "lidarvwSERVER: run. thread could not be created");
      return false;
   }
   return true;
}