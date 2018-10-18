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
#include "videodisplay.h"

#define SSD1351_DEVICEID_OLED_0   1
#define SSD1351_NUM_DEVICEID      1

#define SSD1351_CONTEXT_NULL 0

#define SSD1351_WIDTH  128
#define SSD1351_HEIGHT 128
#define SSD1351_BYTES_PER_PIXEL 2
#define SSD1351_FRAMEBUFFER_SIZE_BYTES 32768

typedef int ssd1351_displayid;

bool ssd1351_ini_open(viddisp_deviceid id);
void ssd1351_ini_close(viddisp_deviceid id);
void ssd1351_opr_display_buffer(viddisp_deviceid id, viddisp_frame_buffer buffer);

#endif