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
#define SUIMENU_MENUID_ROOT 1
#define SUIMENU_EXIT_OFFSET 0
#define SUIMENU_BACK_OFFSET 1
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void suimenu_update_observer(vidcomp_context ctx);
static void suimenu_render_observer(vidcomp_context ctx, pixbuf_context frame_buffer);
static void suimenu_inputmixer_observer(input_event event);
static void suimenu_process_click(input_event event);

static void suimenu_clear();
static void suimenu_draw_title();
static void suimenu_draw_item(int index, char *item_text, bool hilite);

void suimenu_ini_exitbutton();
static void suimenu_exit_observer(suimenu_menuid menu_id, suimenu_itemid item_id);
void suimenu_ini_backbutton();
static void suimenu_back_observer(suimenu_menuid menu_id, suimenu_itemid item_id);

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
   suimenu_item_observer item_observer;
   suimenu_menuid menu_id;
   suimenu_itemid item_id;
   struct SUIMENU_MENU * menu;
};

struct SUIMENU_MENU
{
   suimenu_menuid menu_id;
   char text[SUIMENU_ITEM_MAX_CHARS];
   struct SUIMENU_ITEM items[SUIMENU_MENU_MAX_ITEMS];
   suimenu_state_observer state_observer;
   bool has_exit;
   int num_items;
   int hilite_idx;
   suimenu_menuid called_by_id;
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
   int menu_stack[5];
   int menustack_num_elem;
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
static struct SUIMENU_CONTEXT suimenu_ctx;
static struct SUIMENU_ITEM suimenu_backbtn;
static struct SUIMENU_ITEM suimenu_exitbtn;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
suimenu_context suimenu_ini_open(vidcomp_context video_composer)
{
   suimenu_ctx.display = video_composer;
   vidcomp_add_update_observer(suimenu_ctx.display, suimenu_update_observer);
   vidcomp_add_render_observer(suimenu_ctx.display, suimenu_render_observer);

   cvInitFont(&suimenu_ctx.opencv_font, CV_FONT_HERSHEY_PLAIN, 1.0, 1.0, 0, 1, 4);

   viddisp_context display_device = vidcomp_inf_videodisplay(suimenu_ctx.display);
   suimenu_ctx.menu_width = viddisp_inf_width(display_device);
   suimenu_ctx.menu_height = viddisp_inf_height(display_device);
   suimenu_ctx.item_height = suimenu_menu_item_height;
   suimenu_ctx.text_left = suimenu_menu_text_left;
   CvSize menu_canvas_size = {suimenu_ctx.menu_width, suimenu_ctx.menu_height};
   suimenu_ctx.opencv_buff = cvCreateImage(menu_canvas_size, IPL_DEPTH_8U, 3);
   suimenu_ctx.opencv_buff_rgb565 = cvCreateImage(menu_canvas_size, IPL_DEPTH_8U, 2);

   memcpy (&suimenu_ctx.bk_color, &suimenu_bk_color, sizeof(CvScalar));
   memcpy (&suimenu_ctx.fg_color, &suimenu_fg_color, sizeof(CvScalar));
   memcpy(&suimenu_ctx.hi_color, &suimenu_hi_color, sizeof(CvScalar));
   memcpy(&suimenu_ctx.title_color, &suimenu_title_color, sizeof(CvScalar));

   suimenu_ctx.active_menu = suimenu_cfg_addmenu(&suimenu_ctx, suimenu_mainmenu_name, NULL);

   suimenu_ini_backbutton();
   suimenu_ini_exitbutton();

   return &suimenu_ctx;
}

bool suimenu_inf_showing(suimenu_context menu)
{
   return menu->showing;
}

void suimenu_opr_show(suimenu_context menu, bool show)
{
   menu->showing = show;
}

suimenu_menuid suimenu_cfg_addmenu(suimenu_context menu, const char *text, suimenu_item_observer state_observer)
{
   suimenu_menuid new_id = suimenu_ctx.num_menus + 1;
   struct SUIMENU_MENU *new_menu = &suimenu_ctx.menus[new_id];

   strcpy(new_menu->text, text);
   new_menu->state_observer = state_observer;
   new_menu->menu_id = new_id;

   printf ("suimenu - add menu: %s (%d)\n", new_menu->text, new_menu->menu_id);

   suimenu_ctx.num_menus++;
   return new_id;
}

suimenu_itemid suimenu_cfg_additem(suimenu_context menu_ctx, suimenu_menuid menu_id, const char *text,
                                   suimenu_menuid submenu_id, suimenu_item_observer item_observer)
{
   struct SUIMENU_MENU *menu = &menu_ctx->menus[menu_id];
   struct SUIMENU_ITEM *newitem = &menu->items[menu->num_items];

   strcpy(newitem->text, text);
   newitem->menu = menu;
   newitem->item_id = menu->num_items;
   newitem->sub_menu = submenu_id;
   newitem->item_observer = item_observer;

   if (menu_id == SUIMENU_MENUID_APPLICATION)
   {
      menu_ctx->menus[submenu_id].has_exit = true;
   }

   menu->num_items++;
   return newitem->item_id;
}

void suimenu_cfg_itemtext(suimenu_context menu_ctx, suimenu_menuid menu_id, suimenu_itemid item_id,  const char *text)
{
   struct SUIMENU_MENU *menu = &suimenu_ctx.menus[menu_id];
   struct SUIMENU_ITEM *item = &menu->items[item_id];
   strcpy(item->text, text);
}

void suimenu_inputmixer_injector(input_event event)
{
   //todo: this a hack until the input system is complete.
   suimenu_inputmixer_observer(event);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void suimenu_exit_observer(suimenu_menuid menu_id, suimenu_itemid item_id)
{
   printf("got exit button\n");
   struct SUIMENU_MENU *activemenu = &suimenu_ctx.menus[menu_id];
   activemenu->state_observer(SUIMENU_MENUID_APPLICATION, SUIMENU_STATEID_STOP);
   suimenu_ctx.active_menu = SUIMENU_MENUID_ROOT;
   activemenu = &suimenu_ctx.menus[suimenu_ctx.active_menu];
   activemenu->hilite_idx = 0;
}

static void suimenu_back_observer(suimenu_menuid menu_id, suimenu_itemid item_id)
{
   printf("got back button\n");

   struct SUIMENU_MENU *activemenu = &suimenu_ctx.menus[menu_id];
   if (activemenu->called_by_id != SUIMENU_MENUID_ROOT)
   {
      activemenu = &suimenu_ctx.menus[activemenu->called_by_id];
      suimenu_ctx.active_menu = activemenu->menu_id;
   }
   else
   {
      suimenu_opr_show(&suimenu_ctx, false);
      activemenu->state_observer(SUIMENU_MENUID_APPLICATION, SUIMENU_STATEID_MENU_OFF);
   }
   activemenu->hilite_idx = 0;
}

void suimenu_ini_backbutton()
{
   struct SUIMENU_ITEM *bkbtn = &suimenu_backbtn;

   strcpy(bkbtn->text, "back");
   bkbtn->menu = SUIMENU_MENUID_NULL;
   bkbtn->item_id = SUIMENU_STATEID_MENU_BACK;
   bkbtn->sub_menu = SUIMENU_MENUID_NULL;
   bkbtn->item_observer = suimenu_back_observer;
}

void suimenu_ini_exitbutton()
{
   struct SUIMENU_ITEM *exitbtn = &suimenu_exitbtn;

   strcpy(exitbtn->text, "exit");
   exitbtn->menu = SUIMENU_MENUID_NULL;
   exitbtn->item_id = SUIMENU_STATEID_STOP;
   exitbtn->sub_menu = SUIMENU_MENUID_NULL;
   exitbtn->item_observer = suimenu_exit_observer;
}

static void suimenu_inputmixer_observer(input_event event)
{
   printf("suimenu: got input event. 0x%x\n", inputevt_inf_id(event));

   struct SUIMENU_MENU *activemenu = &suimenu_ctx.menus[suimenu_ctx.active_menu];

   if (inputevt_inf_id(event) == INPUTEVT_EVENTID_UP)
   {
      activemenu->hilite_idx--;
      if (activemenu->hilite_idx < 0)
      {
         activemenu->hilite_idx = activemenu->num_items-1;
         if (suimenu_ctx.active_menu != SUIMENU_MENUID_ROOT)
            activemenu->hilite_idx++;
         if (activemenu->has_exit)
            activemenu->hilite_idx++;
      }
   }
   else if (inputevt_inf_id(event) == INPUTEVT_EVENTID_DOWN)
   {
      activemenu->hilite_idx++;
      if (suimenu_ctx.active_menu == SUIMENU_MENUID_ROOT && activemenu->hilite_idx == activemenu->num_items)
         activemenu->hilite_idx = 0;
      else if (!activemenu->has_exit && activemenu->hilite_idx == activemenu->num_items + 1)
         activemenu->hilite_idx = 0;
      else if (activemenu->hilite_idx == activemenu->num_items+2)
         activemenu->hilite_idx = 0;

   }
   else if (inputevt_inf_id(event) == INPUTEVT_EVENTID_CLICKED)
   {
      suimenu_process_click(event);
   }
}

static void suimenu_process_click(input_event event)
{
   struct SUIMENU_MENU *activemenu = &suimenu_ctx.menus[suimenu_ctx.active_menu];

   int selectedidx = activemenu->hilite_idx;

   if (!suimenu_ctx.showing)
   {
      suimenu_opr_show(&suimenu_ctx, true);
      if (activemenu->state_observer)
         activemenu->state_observer(SUIMENU_MENUID_APPLICATION, SUIMENU_STATEID_MENU_ON);
      return;
   }

   if (suimenu_ctx.showing && selectedidx >= activemenu->num_items)
   {
      //exit or back button
      if (selectedidx == activemenu->num_items)
      {
         if (activemenu->has_exit)
            suimenu_exitbtn.item_observer(suimenu_ctx.active_menu, selectedidx);
         else
            suimenu_backbtn.item_observer(suimenu_ctx.active_menu, selectedidx);
      }
      else if (selectedidx == activemenu->num_items + 1)
      {
         suimenu_backbtn.item_observer(suimenu_ctx.active_menu, selectedidx);
      }
   }
   if (activemenu->items[selectedidx].item_observer != NULL)
   {
      if (suimenu_ctx.active_menu != SUIMENU_MENUID_ROOT)
      {
         if (!suimenu_inf_showing(&suimenu_ctx))
         {
            suimenu_opr_show(&suimenu_ctx, true);
            if (activemenu->state_observer)
               activemenu->state_observer(SUIMENU_MENUID_APPLICATION, SUIMENU_STATEID_MENU_ON);
         }
         else
         {
            int activeid = suimenu_ctx.active_menu;
            int itemid = suimenu_ctx.menus[activeid].hilite_idx;
            suimenu_opr_show(&suimenu_ctx, false);
            activemenu->state_observer(SUIMENU_MENUID_APPLICATION, SUIMENU_STATEID_MENU_OFF);
            activemenu->items[selectedidx].item_observer(activeid, itemid);
            if (activemenu->called_by_id != SUIMENU_MENUID_ROOT)
            {
               while (activemenu->called_by_id != SUIMENU_MENUID_ROOT)
                  activemenu = &suimenu_ctx.menus[activemenu->called_by_id];
               suimenu_ctx.active_menu = activemenu->menu_id;
               selectedidx = 0;
            }
         }
      }
      else
      {
         struct SUIMENU_MENU *submenu = &suimenu_ctx.menus[activemenu->items[selectedidx].sub_menu];
         submenu->hilite_idx = 0;
         submenu->called_by_id = SUIMENU_MENUID_ROOT;
         submenu->state_observer(SUIMENU_MENUID_APPLICATION, SUIMENU_STATEID_START);
         suimenu_opr_show(&suimenu_ctx, false);
      }
   }

   if (activemenu->items[selectedidx].sub_menu != SUIMENU_MENUID_NULL)
   {
      int called_by = activemenu->menu_id;
      suimenu_ctx.active_menu = suimenu_ctx.menus[suimenu_ctx.active_menu].items[selectedidx].sub_menu;
      activemenu = &suimenu_ctx.menus[suimenu_ctx.active_menu];
      if (activemenu->called_by_id != SUIMENU_MENUID_ROOT)
         activemenu->called_by_id = called_by;
      printf("active menu: %s\n", activemenu->text);
   }
}

static void suimenu_update_observer(vidcomp_context ctx)
{
   if (!suimenu_ctx.showing)
      return;

   suimenu_clear();
   suimenu_draw_title();
   struct SUIMENU_MENU * active_menu = &suimenu_ctx.menus[suimenu_ctx.active_menu];
   for (int item_idx=0; item_idx < active_menu->num_items; item_idx++)
   {
      suimenu_draw_item(item_idx, active_menu->items[item_idx].text, item_idx == active_menu->hilite_idx ? true : false);
   }
   if (suimenu_ctx.active_menu != SUIMENU_MENUID_ROOT)
   {
      int next_item = active_menu->num_items;
      if (active_menu->has_exit)
      {
         suimenu_draw_item(next_item, suimenu_exitbtn.text, next_item == active_menu->hilite_idx ? true : false);
         next_item++;
      }

      suimenu_draw_item(next_item, suimenu_backbtn.text, next_item == active_menu->hilite_idx ? true : false);
   }

   cvCvtColor(suimenu_ctx.opencv_buff, suimenu_ctx.opencv_buff_rgb565, CV_BGR2BGR565);
}

static void suimenu_render_observer(vidcomp_context ctx, pixbuf_context frame_buffer)
{
   if (!suimenu_ctx.showing)
      return;
   memcpy(pixbuf_inf_pixels(frame_buffer), suimenu_ctx.opencv_buff_rgb565->imageData, suimenu_ctx.menu_width * suimenu_ctx.menu_height * 2);
}

static void suimenu_clear()
{
   CvRect bkrect = {0, 0, suimenu_ctx.menu_width, suimenu_ctx.menu_height};

   cvRectangleR(suimenu_ctx.opencv_buff, bkrect, suimenu_ctx.bk_color, CV_FILLED, 8, 0);
}

static void suimenu_draw_title()
{
   CvRect bkrect = {0,0, suimenu_ctx.menu_width, suimenu_ctx.item_height};

   cvRectangleR(suimenu_ctx.opencv_buff, bkrect, suimenu_ctx.title_color, CV_FILLED, 8, 0);

   CvSize rendered_size;
   int rendered_baseline;
   cvGetTextSize(suimenu_ctx.menus[suimenu_ctx.active_menu].text, &suimenu_ctx.opencv_font, &rendered_size, &rendered_baseline);

   int left = (suimenu_ctx.menu_width - rendered_size.width) / 2;
   CvPoint text_loc = {left, suimenu_ctx.item_height};
   text_loc.y -= 4;

   cvPutText(suimenu_ctx.opencv_buff, suimenu_ctx.menus[suimenu_ctx.active_menu].text, text_loc, &suimenu_ctx.opencv_font, suimenu_ctx.fg_color);
}

static void suimenu_draw_item(int index, char * item_text, bool hilite)
{
   CvRect bkrect = {0,
                    suimenu_ctx.item_height * (index+1),
                    suimenu_ctx.menu_width,
                    suimenu_ctx.item_height};

   if (hilite)
      cvRectangleR(suimenu_ctx.opencv_buff, bkrect, suimenu_ctx.hi_color, CV_FILLED, 8, 0);

   CvPoint text_loc = {suimenu_ctx.text_left, suimenu_ctx.item_height * (index + 2)};
   text_loc.y -= 4;

   cvPutText(suimenu_ctx.opencv_buff, item_text, text_loc, &suimenu_ctx.opencv_font, suimenu_ctx.fg_color);
}