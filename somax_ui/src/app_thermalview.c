//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: thermalview.c
// DESCRIPTION: Display a heat map of thermal imager data in real time.
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
#include "thermalcamera.h"
#include "somaxui_menu.h"
#include "gimbal.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define THRMVW_QUALITY_HIGH   1
#define THRMVW_QUALITY_MED    0
#define THRMVW_QUALITY_LOW    2
#define THRMVW_QUALITY_FAST   3

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static vidcomp_context vidcomposer;
static void thermalview_update_observer(vidcomp_context ctx);
static void thermalview_render_observer(vidcomp_context ctx, pixbuf_context frame_buffer);
static bool thermalview_server_run();
static void thermalview_init_menu(suimenu_context sui_menu);
static void thermalview_menu_state_observer(suimenu_menuid menu_id, suimenu_itemid state_id);
static void thermalview_menu_item_observer(suimenu_menuid menu_id, suimenu_itemid state_id);

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
static thermcam_framedata_buffer thermcam_framebuffer;
static thermcam_context thermcam;
static IplImage * thermcam_opencv_buff;
static IplImage * thermcam_opencv_buff_resized;
static IplImage *thermcam_opencv_buff_resized_rgb565;
static CvFont thermcam_opencv_font;

static bool thermcam_receiving_frame = false;
static bool thermcam_displaying_frame = false;
static thermcam_framedata_buffer current_framedata;
static bool headless = false;
static bool thrmvw_show;
static bool thrmvw_initialized;
static bool thrmvw_gimbal_enabled;
static int thrmvw_quality;

static suimenu_context thrmvw_menu_ctx;
static char thrmvw_menu_name[] = "thermal view";
static const char thrmvw_menuitem_units[]       = "celsius/faren";
static const char thrmvw_menuitem_snapshot[]    = "take snapshot";
static const char thrmvw_menuitem_gimbal_on[]   = "gimbal on";
static const char thrmvw_menuitem_gimbal_off[]  = "gimbal off";

static suimenu_menuid thrmvw_menuid;
static suimenu_itemid thrmvw_itemid_units;
static suimenu_itemid thrmvw_itemid_snapshot;
static suimenu_itemid thrmvw_itemid_gimbal;

static suimenu_menuid thrmvw_menuid_quality;
static suimenu_itemid thrmvw_itemid_quality_fst;
static suimenu_itemid thrmvw_itemid_quality_low;
static suimenu_itemid thrmvw_itemid_quality_med;
static suimenu_itemid thrmvw_itemid_quality_hi;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int thermalview_init(int argc, char *argv[], suimenu_context main_menu, vidcomp_context video_composer)
{
   int return_code = 0;

   if (thrmvw_initialized)
      return 0;

   printf("initializing thermalview application\n");

   thrmvw_menu_ctx = main_menu;

   vidcomposer = video_composer;
   vidcomp_add_update_observer(vidcomposer, thermalview_update_observer);
   vidcomp_add_render_observer(vidcomposer, thermalview_render_observer);

   thermalview_init_menu(main_menu);

   if (!return_code && !thermalview_server_run())
      return_code = 1;
   else
      thrmvw_initialized = true;

   return return_code;
}

void thermalview_show(bool show)
{
   thrmvw_show = show;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void thermalview_init_menu(suimenu_context menu_ctx)
{
   thrmvw_menuid_quality = suimenu_cfg_addmenu(menu_ctx, "quality >", thermalview_menu_state_observer);
   thrmvw_itemid_quality_fst = suimenu_cfg_additem(menu_ctx, thrmvw_menuid_quality, "fast", SUIMENU_MENUID_NULL, thermalview_menu_item_observer);
   thrmvw_itemid_quality_low = suimenu_cfg_additem(menu_ctx, thrmvw_menuid_quality, "low", SUIMENU_MENUID_NULL, thermalview_menu_item_observer);
   thrmvw_itemid_quality_med = suimenu_cfg_additem(menu_ctx, thrmvw_menuid_quality, "*medium", SUIMENU_MENUID_NULL, thermalview_menu_item_observer);
   thrmvw_itemid_quality_hi  = suimenu_cfg_additem(menu_ctx, thrmvw_menuid_quality, "high", SUIMENU_MENUID_NULL, thermalview_menu_item_observer);

   //application menu - will be displayed when the select button is pressed.
   thrmvw_menuid = suimenu_cfg_addmenu(menu_ctx, thrmvw_menu_name, thermalview_menu_state_observer);
   thrmvw_itemid_units = suimenu_cfg_additem(menu_ctx, thrmvw_menuid, thrmvw_menuitem_units, SUIMENU_MENUID_NULL, thermalview_menu_item_observer);
   thrmvw_itemid_snapshot = suimenu_cfg_additem(menu_ctx, thrmvw_menuid, thrmvw_menuitem_snapshot, SUIMENU_MENUID_NULL, thermalview_menu_item_observer);
   thrmvw_itemid_gimbal = suimenu_cfg_additem(menu_ctx, thrmvw_menuid, thrmvw_menuitem_gimbal_on, SUIMENU_MENUID_NULL, thermalview_menu_item_observer);
   suimenu_cfg_additem(menu_ctx, thrmvw_menuid, "quality", thrmvw_menuid_quality, NULL);


   //add thermal view to the application menu
   suimenu_cfg_additem(menu_ctx, SUIMENU_MENUID_APPLICATION, thrmvw_menu_name, thrmvw_menuid, thermalview_menu_item_observer);
}

static void thermalview_menu_state_observer(suimenu_menuid menu_id, suimenu_stateid state_id)
{
   //start the camera and send data to the oled
   if (menu_id == SUIMENU_MENUID_APPLICATION && state_id == SUIMENU_STATEID_START)
   {
      printf("starting thermalview application\n");
      thermalview_show(true);
   }

   //stop the camera and display update
   if (menu_id == SUIMENU_MENUID_APPLICATION && state_id == SUIMENU_STATEID_STOP)
   {
      printf("stopping thermalview application\n");
      if (thrmvw_gimbal_enabled)
      {
         gimbal_opr_enable_motors(false);
         gimbal_opr_enable_input(false);
         suimenu_cfg_itemtext(thrmvw_menu_ctx, thrmvw_menuid, thrmvw_itemid_gimbal, thrmvw_menuitem_gimbal_on);
      }
      thermalview_show(false);
   }

   //displaying the menu, save band width by stopping thrmvw_display
   if (menu_id == SUIMENU_MENUID_APPLICATION && state_id == SUIMENU_STATEID_MENU_ON)
   {
      printf("thermalview application - menu on\n");
      thermalview_show(false);
   }
   //displaying the menu, save band width by stopping thrmvw_display
   if (menu_id == SUIMENU_MENUID_APPLICATION && state_id == SUIMENU_STATEID_MENU_OFF)
   {
      printf("thermalview application - menu off\n");
      thermalview_show(true);
   }
}

static void thermalview_menu_item_observer(suimenu_menuid menu_id, suimenu_itemid item_id)
{
   //enable / disable gimbal
   if (menu_id == thrmvw_menuid && item_id == thrmvw_itemid_gimbal)
   {
      if (!thrmvw_gimbal_enabled)
      {
         printf("thermalview application enable gimbal\n");
         thrmvw_gimbal_enabled = true;
         gimbal_opr_enable_motors(true);
         gimbal_opr_enable_input(true);
         suimenu_cfg_itemtext(thrmvw_menu_ctx, thrmvw_menuid, thrmvw_itemid_gimbal, thrmvw_menuitem_gimbal_off);
      }
      else
      {
         printf("thermalview application disable gimbal\n");
         thrmvw_gimbal_enabled = false;
         gimbal_opr_enable_motors(false);
         gimbal_opr_enable_input(false);
         suimenu_cfg_itemtext(thrmvw_menu_ctx, thrmvw_menuid, thrmvw_itemid_gimbal, thrmvw_menuitem_gimbal_on);
      }
   }

   if (menu_id == thrmvw_menuid_quality)
   {
      if (thrmvw_quality == THRMVW_QUALITY_HIGH)
         suimenu_cfg_itemtext(thrmvw_menu_ctx, thrmvw_menuid_quality, thrmvw_itemid_quality_hi, "high");
      else if (thrmvw_quality == THRMVW_QUALITY_MED)
         suimenu_cfg_itemtext(thrmvw_menu_ctx, thrmvw_menuid_quality, thrmvw_itemid_quality_med, "medium");
      else if (thrmvw_quality == THRMVW_QUALITY_LOW)
         suimenu_cfg_itemtext(thrmvw_menu_ctx, thrmvw_menuid_quality, thrmvw_itemid_quality_low, "low");
      else if (thrmvw_quality == THRMVW_QUALITY_FAST)
         suimenu_cfg_itemtext(thrmvw_menu_ctx, thrmvw_menuid_quality, thrmvw_itemid_quality_low, "fast");

      if (item_id == thrmvw_itemid_quality_hi)
      {
         printf("got hi quality click\n");
         suimenu_cfg_itemtext(thrmvw_menu_ctx, thrmvw_menuid_quality, thrmvw_itemid_quality_hi, "*high");
         thrmvw_quality = THRMVW_QUALITY_HIGH;
      }
      else if (item_id == thrmvw_itemid_quality_med)
      {
         printf("got med quality click\n");
         suimenu_cfg_itemtext(thrmvw_menu_ctx, thrmvw_menuid_quality, thrmvw_itemid_quality_med, "*medium");
         thrmvw_quality = THRMVW_QUALITY_MED;
      }
      else if (item_id == thrmvw_itemid_quality_low)
      {
         printf("got low quality click\n");
         suimenu_cfg_itemtext(thrmvw_menu_ctx, thrmvw_menuid_quality, thrmvw_itemid_quality_low, "*low");
         thrmvw_quality = THRMVW_QUALITY_LOW;
      }
      else if (item_id == thrmvw_itemid_quality_fst)
      {
         printf("got fast quality click\n");
         suimenu_cfg_itemtext(thrmvw_menu_ctx, thrmvw_menuid_quality, thrmvw_itemid_quality_fst, "*fast");
         thrmvw_quality = THRMVW_QUALITY_FAST;
      }
   }
}

void thermalview_update_draw_crosshair(IplImage *canvas)
{
   CvPoint begin = {63-5, 63};
   CvPoint end = {63+5, 63};
   CvScalar color = {255,255,255};

   cvLine(canvas, begin, end, color, 2, 0);

   begin.x = 63; begin.y = 63-5;
   end.x = 63; end.y = 63+5;
   cvLine(canvas, begin, end, color, 2, 0);
}

void thermalview_update_draw_temp(IplImage *canvas, float avg_temp, float target_temp)
{
   CvPoint text_loc = {0, 15};
   CvScalar text_color = {255, 255, 255};
   char text_msg[16];

   sprintf(text_msg, "%.1f/%.1f F", target_temp, avg_temp);

   cvPutText(canvas, text_msg, text_loc, &thermcam_opencv_font, text_color);
}

void thermalview_update_observer(vidcomp_context ctx)
{
   if (!thrmvw_show)
      return;

   if (thermcam_receiving_frame)
      return;

   thermcam_displaying_frame = true;
   if (!thermcam_opencv_buff)
   {
      CvSize size = {8, 8};
      thermcam_opencv_buff = cvCreateImage(size, IPL_DEPTH_8U, 3);
      size.height = size.width = 128;
      thermcam_opencv_buff_resized = cvCreateImage(size, IPL_DEPTH_8U, 3);
      thermcam_opencv_buff_resized_rgb565 = cvCreateImage(size, IPL_DEPTH_8U, 2);
      cvInitFont(&thermcam_opencv_font, CV_FONT_HERSHEY_PLAIN, 1.0, 1.0, 0, 1, 4);
   }

   float min_temp = 100.0;
   float avg_temp = 0.0;
   float max_temp = 0.0;
   float target_temp = 0.0;
   for (int i = 0; i < 64; i++)
   {
      if (current_framedata[i] < min_temp)
         min_temp = current_framedata[i];
      else if (current_framedata[i] > max_temp)
         max_temp = current_framedata[i];
      avg_temp += current_framedata[i];
   }
   avg_temp /= 64.0;
   target_temp = current_framedata[(8 * 3) + 3];
   if (target_temp < current_framedata[(8 * 3) + 4])
      target_temp = current_framedata[(8 * 3) + 4];
   if (target_temp < current_framedata[(8 * 4) + 3])
      target_temp = current_framedata[(8 * 4) + 3];
   if (target_temp < current_framedata[(8 * 4) + 4])
      target_temp = current_framedata[(8 * 4) + 4];

   for (int i = 0; i < 64; i++)
   {
      float temp = current_framedata[i];

      uint8_t r, g, b;

      r = 0;
      g = 0;
      b = 0;

      //float max_temp = 100.0;
      //float min_temp = 30.0;
      float temp_cutoff = avg_temp;
      if (min_temp / max_temp > .90)
         temp_cutoff = max_temp + 1;
      if (temp >= temp_cutoff)
      {
         float scalar = ((temp - temp_cutoff) / (max_temp - temp_cutoff));
         b = 8.0 * (1.0 - scalar);
         r = 15.0 + ((max_temp - avg_temp) * scalar);
      }
      else
      {
         float scalar = ((temp - min_temp) / (temp_cutoff - min_temp));
         r = 8.0 * scalar;
         b = 15.0; // + ((avg_temp - min_temp) * scalar);
      }

      thermcam_opencv_buff->imageData[(i * 3)] = b;
      thermcam_opencv_buff->imageData[(i * 3) + 1] = r;
      thermcam_opencv_buff->imageData[(i * 3) + 2] = g;
   }

   cvTranspose(thermcam_opencv_buff, thermcam_opencv_buff);
   if (headless)
      cvFlip(thermcam_opencv_buff, thermcam_opencv_buff, 1);
   cvResize(thermcam_opencv_buff, thermcam_opencv_buff_resized, CV_INTER_CUBIC);
   thermalview_update_draw_temp(thermcam_opencv_buff_resized, avg_temp, target_temp);
   thermalview_update_draw_crosshair(thermcam_opencv_buff_resized);
   cvCvtColor(thermcam_opencv_buff_resized, thermcam_opencv_buff_resized_rgb565, CV_RGB2BGR565);
}

void thermalview_render_observer(vidcomp_context ctx, pixbuf_context frame_buffer)
{
   if (!thrmvw_show)
      return;

   memcpy(pixbuf_inf_pixels(frame_buffer), thermcam_opencv_buff_resized_rgb565->imageData, 128 * 128 * 2);
   thermcam_displaying_frame = false;
}

void thermalview_thermcam_observer(thermcam_context ctx, thermcam_observer_id observer_id, thermcam_framedata_buffer return_buffer)
{
   if (thermcam_displaying_frame)
      return;

   thermcam_receiving_frame = true;
   memcpy(current_framedata, return_buffer, THERMCAM_FRAMEBUFFER_MAX_SIZE * sizeof(float));
   thermcam_receiving_frame = false;
}

static void *thermalview_server(void *arg)
{
   thermcam = thermcam_open(THERMCAMID_GIMBAL_LEFT);

   thermcam_cfg_output_units(thermcam, THERMCAM_OUTPUTUNITS_FARENHEIT);
   thermcam_cfg_observer_framedata(thermcam, 0, thermalview_thermcam_observer, &thermcam_framebuffer);

   thermcam_run(thermcam, THERMCAM_NUMFRAMES_CONTINUOUS);

   return 0;
}

static bool thermalview_server_run()
{
   pthread_t thread_id;
   int error = pthread_create(&thread_id, NULL, &thermalview_server, NULL);
   if (error)
   {
      somax_log_add(SOMAX_LOG_ERR, "THERMCAMSERVER: run. thread could not be created");
      return false;
   }
   return true;
}