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
#include "opencv2/imgproc/imgproc_c.h"

#include "somax.h"
#include "videocomposer.h"
#include "thermalcamera.h"

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
vidcomp_context vidcomposer;
static void thermalview_update_observer(vidcomp_context ctx);
static void thermalview_render_observer(vidcomp_context ctx, pixbuf_context frame_buffer);
static bool thermalview_server_run();

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

static bool thermcam_receiving_frame = false;
static bool thermcam_displaying_frame = false;
static thermcam_framedata_buffer current_framedata;
static bool headless = false;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int thermalview_run(int argc, char *argv[])
{
   int return_code = 0;

   vidcomposer = vidcomp_ini_open(VIDCOMP_DISPLAYID_FRAME_PRIMARY);
   vidcomp_add_update_observer(vidcomposer, thermalview_update_observer);
   vidcomp_add_render_observer(vidcomposer, thermalview_render_observer);

   if (!return_code && !thermalview_server_run())
      return_code = 1;

   if (!return_code && !vidcomp_opr_run(vidcomposer, 5))
      return_code = 1;

   return return_code;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void thermalview_update_observer(vidcomp_context ctx)
{
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
   }

   float min_temp = 100.0;
   float avg_temp = 0.0;
   float max_temp = 0.0;
   for (int i = 0; i < 64; i++)
   {
      if (current_framedata[i] < min_temp)
         min_temp = current_framedata[i];
      else if (current_framedata[i] > max_temp)
         max_temp = current_framedata[i];
      avg_temp += current_framedata[i];
   }
   avg_temp /= 64.0;

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
      if (temp >= temp_cutoff)
      {
         float scalar = ((temp - temp_cutoff) / (max_temp - temp_cutoff));
         b = 6.0 * (1.0 - scalar);
         r = 15.0 + (10.0 * scalar);
         //g = 32 * scalar;
      }
      else
      {
         float scalar = ((temp - min_temp) / (temp_cutoff - min_temp));
         r = 6.0 * scalar;
         b = 15.0 + (10.0 * scalar);
         //g = 5 * scalar;
      }

      // thermcam_opencv_buff->imageData[(i * 3)] = b;
      // thermcam_opencv_buff->imageData[(i * 3) + 1] = r;
      // thermcam_opencv_buff->imageData[(i * 3) + 2] = g;

      thermcam_opencv_buff->imageData[(i * 3)] = b;
      thermcam_opencv_buff->imageData[(i * 3) + 1] = r;
      thermcam_opencv_buff->imageData[(i * 3) + 2] = g;
   }


   cvTranspose(thermcam_opencv_buff, thermcam_opencv_buff);
   if (headless)
      cvFlip(thermcam_opencv_buff, thermcam_opencv_buff, 1);
   cvResize(thermcam_opencv_buff, thermcam_opencv_buff_resized, CV_INTER_CUBIC);
   cvCvtColor(thermcam_opencv_buff_resized, thermcam_opencv_buff_resized_rgb565, CV_RGB2BGR565);
}

void thermalview_render_observer(vidcomp_context ctx, pixbuf_context frame_buffer)
{
   memcpy(pixbuf_inf_pixels(frame_buffer), thermcam_opencv_buff_resized_rgb565->imageData, 128 * 128 * 2);
   thermcam_displaying_frame = false;
}

// void thermalview_update_observer(vidcomp_context ctx)
// {
//    if (thermcam_receiving_frame)
//       return;

//    thermcam_displaying_frame = true;
//    if (!thermcam_displaybuffer)
//    {
//       thermcam_displaybuffer = viddisp_opr_newpixbuf(vidcomp_inf_videodisplay(vidcomposer), 128, 128);
//       CvSize size = {8, 8};
//       thermcam_opencv_buff = cvCreateImage(size, IPL_DEPTH_16U, 3);
//    }

//    smx_byte *pixels = pixbuf_inf_pixels(thermcam_displaybuffer);
//    float min_temp = 100.0;
//    float avg_temp = 0.0;
//    float max_temp = 0.0;
//    for (int i = 0; i < 64; i++)
//    {
//       if (current_framedata[i] < min_temp)
//          min_temp = current_framedata[i];
//       else if (current_framedata[i] > max_temp)
//          max_temp = current_framedata[i];
//       avg_temp += current_framedata[i];
//    }
//    avg_temp /= 64.0;

//    for (int i = 0; i < 64; i++)
//    {
//       float temp = current_framedata[i];
//       int col_stride = 2 * 16;
//       int row_stride = col_stride * 128;
//       int row_bytes = 128 * 2;
//       int pixel_bytes = 2;

//       int row = i % 8;
//       int col = i / 8;
//       uint8_t r, g, b;

//       r = 0;
//       g = 0;
//       b = 0;

//       uint16_t color;
//       //float max_temp = 100.0;
//       //float min_temp = 30.0;
//       float temp_cutoff = avg_temp;
//       if (temp >= temp_cutoff)
//       {
//          float color_diff = 191.0 * ((temp - temp_cutoff) / (max_temp - temp_cutoff));
//          if (color_diff > 191.0 || color_diff < 0)
//             printf("color too hihgh\n");
//          r = (uint8_t)(64.0 + color_diff);
//       }
//       else
//       {
//          float color_diff = 32.0 * ((temp_cutoff - temp) / (temp_cutoff - min_temp));
//          b = 42 - (uint8_t)color_diff;
//       }

//       color = (r >> 3) & 0x1F;
//       color <<= 6;
//       color |= (g >> 2) & 0x3F;
//       color <<= 5;
//       color |= (b >> 3) & 0x1F;
//       int location;
//       for (int row_offset = 0; row_offset < 16; row_offset++)
//       {
//          for (int col_offset = 0; col_offset < 16; col_offset++)
//          {
//             location = (row_stride * row) + (row_bytes * row_offset) + (col_stride * col) + (pixel_bytes * col_offset);
//             uint8_t *pixel_bytes = &pixels[location];
//             uint8_t color_lo = (uint8_t)(color & 0xff);
//             uint8_t color_hi = (uint8_t)((color >> 8) & 0xff);
//             pixel_bytes[0] = color_hi;
//             pixel_bytes[1] = color_lo;
//          }
//       }
//    }
// }

// void thermalview_render_observer(vidcomp_context ctx, pixbuf_context frame_buffer)
// {
//    memcpy(pixbuf_inf_pixels(frame_buffer), pixbuf_inf_pixels(thermcam_displaybuffer), 128 * 128 * 2);
//    thermcam_displaying_frame = false;
// }

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
   thermcam = thermcam_open(THERMCAMID_GIMBAL);

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