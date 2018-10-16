#ifndef __videodisplay_ssd1351_h__
#define __videodisplay_ssd1351_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: color_oled.h
// DESCRIPTION: driver for color oled display
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "somax.h"

#define SSD1351_NUM_DISPLAYID      1
#define SSD1351_DISPLAYID_OLED15   1

#define SSD1351_CONTEXT_NULL 0

#define SSD1351_WIDTH  128
#define SSD1351_HEIGHT 128
#define SSD1351_BYTES_PER_PIXEL 2
#define SSD1351_FRAMEBUFFER_SIZE_BYTES 32768

typedef int ssd1351_displayid;
struct SSD1351_CONTEXT;
typedef struct SSD1351_CONTEXT* ssd1351_context;

typedef smx_byte ssd1351_frame_buffer[SSD1351_FRAMEBUFFER_SIZE_BYTES];

ssd1351_context ssd1351_open(ssd1351_displayid id);
void  ssd1351_close(ssd1351_context camt);
void ssd1351_display_buffer(ssd1351_context display, ssd1351_frame_buffer buffer);

#endif