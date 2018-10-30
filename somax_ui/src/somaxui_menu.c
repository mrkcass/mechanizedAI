//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: somaxui_menu.c
// DESCRIPTION: Somax user interface menu.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <stdio.h>

#include "opencv2/core/core_c.h"
#include "opencv2/core/types_c.h"
#include "opencv2/core/types_c.h"
#include "opencv2/imgproc/imgproc_c.h"

#include "somax.h"
#include "videocomposer.h"
#include "inputmixer.h"
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
static void suimenu_update_observer(vidcomp_context ctx);
static void suimenu_render_observer(vidcomp_context ctx, pixbuf_context frame_buffer);
static void suimenu_inputmixer_observer(input_event event);

static void suimenu_clear();
static void suimenu_draw_title();
static void suimenu_draw_item(int index, char *item_text, bool hilite);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA STRUCTURES
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct SUIMENU_MENU;

struct SUIMENU_ITEM
{
   char text[SUIMENU_ITEM_MAX_CHARS];
   suimenu_menuid sub_menu;
   suimenu_observer menu_observer;
   suimenu_menuid menu_id;
   suimenu_itemid item_id;
   struct SUIMENU_MENU * menu;
};

struct SUIMENU_MENU
{
   char text[SUIMENU_ITEM_MAX_CHARS];
   struct SUIMENU_ITEM items[SUIMENU_MENU_MAX_ITEMS];
   int num_items;
   int hilite_idx;
};

struct SUIMENU_CONTEXT
{
   vidcomp_context display;
   IplImage *opencv_buff;
   IplImage *opencv_buff_rgb565;
   CvFont opencv_font;
   bool showing;
   CvScalar bk_color;
   CvScalar fg_color;
   CvScalar hi_color;
   CvScalar title_color;
   int menu_width;
   int menu_height;
   int item_height;
   int text_left;

   struct SUIMENU_MENU menus[SUIMENU_MENU_MAX_ITEMS];
   int num_menus;

   int active_menu;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

CvScalar suimenu_bk_color = {2, 107, 9};
CvScalar suimenu_fg_color = {255, 255, 255};
CvScalar suimenu_hi_color = {1, 183, 13};
CvScalar suimenu_title_color = {0, 64, 0};
int suimenu_menu_item_height = 16;
int suimenu_menu_text_left = 5;
char suimenu_mainmenu_name[] = "Somax Apps";
static struct SUIMENU_CONTEXT rootmenu;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
suimenu_context suimenu_ini_open(vidcomp_context video_composer)
{
   rootmenu.display = video_composer;
   vidcomp_add_update_observer(rootmenu.display, suimenu_update_observer);
   vidcomp_add_render_observer(rootmenu.display, suimenu_render_observer);

   cvInitFont(&rootmenu.opencv_font, CV_FONT_HERSHEY_PLAIN, 1.0, 1.0, 0, 1, 4);

   viddisp_context display_device = vidcomp_inf_videodisplay(rootmenu.display);
   rootmenu.menu_width = viddisp_inf_width(display_device);
   rootmenu.menu_height = viddisp_inf_height(display_device);
   rootmenu.item_height = suimenu_menu_item_height;
   rootmenu.text_left = suimenu_menu_text_left;
   CvSize menu_canvas_size = {rootmenu.menu_width, rootmenu.menu_height};
   rootmenu.opencv_buff = cvCreateImage(menu_canvas_size, IPL_DEPTH_8U, 3);
   rootmenu.opencv_buff_rgb565 = cvCreateImage(menu_canvas_size, IPL_DEPTH_8U, 2);

   memcpy (&rootmenu.bk_color, &suimenu_bk_color, sizeof(CvScalar));
   memcpy (&rootmenu.fg_color, &suimenu_fg_color, sizeof(CvScalar));
   memcpy(&rootmenu.hi_color, &suimenu_hi_color, sizeof(CvScalar));
   memcpy(&rootmenu.title_color, &suimenu_title_color, sizeof(CvScalar));

   rootmenu.active_menu = suimenu_cfg_addmenu(&rootmenu, suimenu_mainmenu_name);

   return &rootmenu;
}

bool suimenu_inf_showing(suimenu_context menu)
{
   return menu->showing;
}

void suimenu_opr_show(suimenu_context menu, bool show)
{
   menu->showing = show;
}

suimenu_menuid suimenu_cfg_addmenu(suimenu_context menu, char *text)
{
   suimenu_menuid new_id = rootmenu.num_menus + 1;
   struct SUIMENU_MENU *new_menu = &rootmenu.menus[new_id];

   strcpy(new_menu->text, text);

   rootmenu.num_menus++;
   return new_id;
}

suimenu_itemid suimenu_cfg_additem(suimenu_context menu_ctx, suimenu_menuid menu_id, char *text,
                                   suimenu_menuid submenu_id, suimenu_observer selected_observer)
{
   struct SUIMENU_MENU *menu = &menu_ctx->menus[menu_id];
   struct SUIMENU_ITEM *new_item = &menu->items[menu->num_items];

   strcpy(new_item->text, text);
   new_item->menu = menu;
   new_item->item_id = menu->num_items;
   new_item->sub_menu = submenu_id;

   menu->num_items++;
   return new_item->item_id;
}

void suimenu_inputmixer_injector(input_event event)
{
   suimenu_inputmixer_observer(event);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void suimenu_inputmixer_observer(input_event event)
{
   if (!rootmenu.showing)
      return;

   printf("suimenu: got input event. 0x%x\n", inputevt_inf_id(event));

   if (inputevt_inf_id(event) == INPUTEVT_EVENTID_UP)
   {
      rootmenu.menus[rootmenu.active_menu].hilite_idx--;
      if (rootmenu.menus[rootmenu.active_menu].hilite_idx < 0)
         rootmenu.menus[rootmenu.active_menu].hilite_idx = rootmenu.menus[rootmenu.active_menu].num_items -1;
   }
   else if (inputevt_inf_id(event) == INPUTEVT_EVENTID_DOWN)
   {
      rootmenu.menus[rootmenu.active_menu].hilite_idx++;
      if (rootmenu.menus[rootmenu.active_menu].hilite_idx >= rootmenu.menus[rootmenu.active_menu].num_items)
         rootmenu.menus[rootmenu.active_menu].hilite_idx = 0;
   }
}

static void suimenu_update_observer(vidcomp_context ctx)
{
   if (!rootmenu.showing)
      return;

   suimenu_clear();
   suimenu_draw_title();
   struct SUIMENU_MENU * active_menu = &rootmenu.menus[rootmenu.active_menu];
   for (int item_idx=0; item_idx < active_menu->num_items; item_idx++)
   {
      suimenu_draw_item(item_idx, active_menu->items[item_idx].text, item_idx == active_menu->hilite_idx ? true : false);
   }
   cvCvtColor(rootmenu.opencv_buff, rootmenu.opencv_buff_rgb565, CV_BGR2BGR565);
}

static void suimenu_render_observer(vidcomp_context ctx, pixbuf_context frame_buffer)
{
   if (!rootmenu.showing)
      return;
   memcpy(pixbuf_inf_pixels(frame_buffer), rootmenu.opencv_buff_rgb565->imageData, rootmenu.menu_width * rootmenu.menu_height * 2);
}

static void suimenu_clear()
{
   CvRect bkrect = {0, 0, rootmenu.menu_width, rootmenu.menu_height};

   cvRectangleR(rootmenu.opencv_buff, bkrect, rootmenu.bk_color, CV_FILLED, 8, 0);
}

static void suimenu_draw_title()
{
   CvRect bkrect = {0,0, rootmenu.menu_width, rootmenu.item_height};

   cvRectangleR(rootmenu.opencv_buff, bkrect, rootmenu.title_color, CV_FILLED, 8, 0);

   CvSize rendered_size;
   int rendered_baseline;
   cvGetTextSize(rootmenu.menus[rootmenu.active_menu].text, &rootmenu.opencv_font, &rendered_size, &rendered_baseline);

   int left = (rootmenu.menu_width - rendered_size.width) / 2;
   CvPoint text_loc = {left, rootmenu.item_height};
   text_loc.y -= 4;

   cvPutText(rootmenu.opencv_buff, rootmenu.menus[rootmenu.active_menu].text, text_loc, &rootmenu.opencv_font, rootmenu.fg_color);
}

static void suimenu_draw_item(int index, char * item_text, bool hilite)
{
   CvRect bkrect = {0,
                    rootmenu.item_height * (index+1),
                    rootmenu.menu_width,
                    rootmenu.item_height};

   if (hilite)
      cvRectangleR(rootmenu.opencv_buff, bkrect, rootmenu.hi_color, CV_FILLED, 8, 0);

   CvPoint text_loc = {rootmenu.text_left, rootmenu.item_height * (index + 2)};
   text_loc.y -= 4;

   cvPutText(rootmenu.opencv_buff, item_text, text_loc, &rootmenu.opencv_font, rootmenu.fg_color);
}