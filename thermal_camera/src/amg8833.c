//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: amg8833.c
// DESCRIPTION: Driver for Panasonic AMG8833 thermal imaging camera.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "amg8833.h"
#include "somax.h"
#include "i2c_interface.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//AMG8833 registers and configuration constants / data
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//power control register
#define AMG_REG_POWER_CONTROL          0x00
#define AMG_POWER_MODE_NORMAL          0x00
#define AMG_POWER_MODE_SLEEP           0x01
#define AMG_POWER_MODE_STANDBY_60      0x20
#define AMG_POWER_MODE_STANDBY_10      0x21
static const char amg_pwr_mode_strings[][32] =
{
   "NORMAL",
   "SLEEP",
   "STANDBY UPDATE EVERY 60sec",
   "STANDBY UPDATE EVERY 10sec",
};
//reset register
#define AMG_REG_RESET                  0x01
#define AMG_RESET_HARD                 0x3F
#define AMG_RESET_SOFT                 0x30
//frame rate register
#define AMG_REG_FRAMERATE              0x02
#define AMG_FRAMERATE_BMASK            0b00000001
#define AMG_FRAMERATE_BSHIFT           0
#define AMG_FRAMERATE_10FPS            0x00
#define AMG_FRAMERATE_1FPS             0x01
static const char amg_framerate_strings[][32] =
{
   "10 FPS",
   "1 FPS",
};
//interrupt control register
#define AMG_REG_INTERRUPT_CONTROL      0x03
      //enable bits
#define AMG_INT_ENABLE_BMASK           0b00000001
#define AMG_INT_ENABLE_BSHIFT          0
#define AMG_INT_ENABLE_READ            0x00
#define AMG_INT_ENABLE_RAISE           0x01
static const char amg_interruptenable_strings[][32] =
{
   "read only interrupts",
   "raising interrupts",
};
      //mode bits
#define AMG_INT_MODE_BMASK             0b00000010
#define AMG_INT_MODE_BSHIFT            1
static const char amg_interruptmode_strings[][32] =
{
   "Differential",
   "Absolute",
};
//status register
#define AMG_REG_STATUS                 0x04
   //interrupt status accumulator bits
#define AMG_INT_MODE_BMASK             0b00000010
#define AMG_INT_MODE_BSHIFT            1
#define AMG_INT_NOTRAISED              0x00
#define AMG_INT_RAISED                 0x01
   //device temperature overflow bits

//thermresistor (device temperature) register
#define AMG_REG_THERMRESISTOR             0x0E
#define AMG_REG_THERMRESISTOR_MSB         0x0F
#define AMG_THERMRESISTORMSB_SIGN_BMASK   0b00001000
#define AMG_THERMRESISTORMSB_SIGN_BSHIFT  3
#define AMG_THERMRESISTORMSB_MSB_BMASK    0b00000111
#define AMG_THERMRESISTORMSB_MSB_BSHIFT   0
#define AMG_THERMRESISTOR_RESOLUTION      .0625

//pixel data registers
#define AMG_BYTES_PER_PIXEL           2
#define AMG_REG_PIXEL_ROW_0           0x80
#define AMG_RAWPIXEL_SIGN_BITMASK     0b0000100000000000
#define AMG_RAWPIXEL_VALUE_BITMASK    0b0000011111111111


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define AMG8833_MAX_CONTEXTS 16


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA STRUCTURES
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct AMG8833_DEVICE_PROPERTIES
{
   amg8833_id device_id;
   char str_device_id[MAX_STR_DEVICEID_LEN];
   int i2c_bus;
   int i2c_address;
};

struct AMG8833_CONTEXT
{
   i2c_context i2c;
   int context_id;
   amg8833_id device_id;
   AMG8833_FRAMEDATA_CALLBACK framedata_observer;
   int num_frames;
   int frame_counter;
   uint8_t * frame_buffer_raw;
   float * frame_buffer;
   bool using_external_framebuffer;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static struct AMG8833_DEVICE_PROPERTIES amg8833_devices[AMG8833_NUM_DEVICES] =
{
   {
      AMG8833_DEVICEID_1,
      "AMG8833_DEVICEID_1",
      I2C_BUSID_1,
      0x69,
   },
};

static struct AMG8833_CONTEXT contexts[AMG8833_MAX_CONTEXTS];
static int amg8833_num_contexts;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void amg8833_run_update_framedata_observer(int context_id);

amg8833_context amg8833_open(amg8833_id id)
{
   if (amg8833_num_contexts >= AMG8833_MAX_CONTEXTS)
   {
      somax_log_add(SOMAX_LOG_ERR, "AMG8833 : Open failed. No free contexts slots.");
      return AMG8833_NULL_CONTEXT;
   }

   if (id < 0 || id > AMG8833_NUM_DEVICES)
   {
      somax_log_add(SOMAX_LOG_ERR, "AMG8833 : Open failed. Unknown device id %d.", id);
      return AMG8833_NULL_CONTEXT;
   }

   struct AMG8833_CONTEXT* ctx = &contexts[amg8833_num_contexts];
   ctx->context_id = amg8833_num_contexts;
   ctx->i2c = i2c_open(amg8833_devices[id].i2c_bus, amg8833_devices[id].i2c_address);
   ctx->device_id = id;

   i2c_set_frequency(ctx->i2c, I2C_FREQUENCY_400KHZ);

   i2c_reg_write_byte(ctx->i2c, AMG_REG_RESET, AMG_RESET_HARD);
   usleep(10 * U_MILLISECOND);
   i2c_reg_write_byte(ctx->i2c, AMG_REG_POWER_CONTROL, AMG_POWER_MODE_NORMAL);
   usleep(10 * U_MILLISECOND);
   i2c_reg_write_byte(ctx->i2c, AMG_REG_POWER_CONTROL, AMG_POWER_MODE_SLEEP);
   usleep(100 * U_MILLISECOND);

   amg8833_num_contexts++;
   return ctx;
}

void amg8833_close(amg8833_context ctx)
{
   if (ctx == NULL)
      return;
   if (ctx->frame_buffer_raw)
      free (ctx->frame_buffer_raw);
   if (ctx->frame_buffer && !ctx->using_external_framebuffer)
      free(ctx->frame_buffer);
   memset(ctx, 0, sizeof(struct AMG8833_CONTEXT));
   amg8833_num_contexts--;
}

void amg8833_info(amg8833_context ctx)
{
   somax_mainboard_id_t id = somax_mainboard_id();
   int somax_bus = amg8833_devices[ctx->device_id].i2c_bus;
   int main_bus = i2c_somax_bus_id_to_mainboard_id(somax_bus);
   int address = amg8833_devices[ctx->device_id].i2c_address;
   printf("   Somax I2C bus:    %d\n", somax_bus);
   printf("   %s I2C bus:   %d\n", somax_mainboard_id_to_string(id), main_bus);
   printf("   I2C address:      0x%X\n", address);

   i2c_dev_write_byte(ctx->i2c, 0);
   int chip_id = i2c_dev_read_byte(ctx->i2c);
   if (chip_id)
      printf("   ChipID:           0x%X\n", chip_id);
   else
      printf("   ChipID:           not supported\n");

   int powermode = i2c_reg_read_byte(ctx->i2c, AMG_REG_POWER_CONTROL);
   if (powermode & AMG_POWER_MODE_STANDBY_60)
      powermode = 2 + (powermode & AMG_POWER_MODE_STANDBY_60) ;
   printf("   Power Mode:       0x%X - %s\n", powermode, amg_pwr_mode_strings[powermode]);

   int framerate = i2c_reg_read_byte(ctx->i2c, AMG_REG_FRAMERATE);
   printf("   Frame rate:       0x%X - %s\n", framerate, amg_framerate_strings[framerate]);

   float dev_temp = amg8833_device_temperature(ctx);
   float dev_temp_faren = somax_convert_celsius_to_farenheit(dev_temp);
   printf("   Temperature C(F)  %3.2f (%3.2f)\n", dev_temp, dev_temp_faren);

   int ints_enabled = (int)amg8833_interrupts_enabled(ctx);
   printf("   Interrupts:       0x%x - %s\n", ints_enabled, amg_interruptenable_strings[ints_enabled]);

   int ints_mode = (int)amg8833_interrupt_mode(ctx);
   printf("   Interrupt Mode:   0x%x - %s\n", ints_mode, amg_interruptmode_strings[ints_mode]);
}

float amg8833_device_temperature(amg8833_context ctx)
{
   uint8_t data[2];

   i2c_reg_read_many(ctx->i2c, AMG_REG_THERMRESISTOR, data, 2);
   int sign = data[1] & AMG_THERMRESISTORMSB_SIGN_BMASK;
   int temp_msb = data[1] & AMG_THERMRESISTORMSB_MSB_BMASK;
   int temp = temp_msb << 1 | data[0];
   float ftemp = (float)temp * AMG_THERMRESISTOR_RESOLUTION;

   if (sign)
      return -ftemp;

   return ftemp;
}

// Specifiy a function to be called when an updated pixel data is ready.
// PARAM: ctx - AMG8833 context.
// PARAM: callback - function to call when a new frame of pixels is ready
// PARAM: frame_buffer - a data buffer that will hold new frame pixel data. if not
//           provided, a buffer will be allocated using malloc which will be sent to
//           the observer by way of 'callbk'. any buffer provided, no matter the storage
//           class is the property of the caller and should be destroyed accordingly
//
//if callback bufffer is NULL, use the driver buffer.
void amg8833_output_callbk_framedata(amg8833_context ctx, AMG8833_FRAMEDATA_CALLBACK callbk, AMG8833_FRAMEDATA_BUFFER frame_buffer)
{
   ctx->framedata_observer = callbk;
   if (frame_buffer && ctx->frame_buffer && !ctx->using_external_framebuffer)
      free(ctx->frame_buffer);
   if (frame_buffer)
   {
      ctx->frame_buffer = frame_buffer;
      ctx->using_external_framebuffer = true;
   }
   else if (!ctx->frame_buffer)
   {
      ctx->frame_buffer = (float *)malloc(AMG8833_ARRAY_SIZE * sizeof(float));
   }

   if (!ctx->frame_buffer_raw)
      ctx->frame_buffer_raw = (uint8_t *)malloc(AMG8833_ARRAY_SIZE * AMG_BYTES_PER_PIXEL);
}

// blocking call that runs an observer update loop.
// PARAM: amg8833 - AMG8833 context
// PARAM: num_frames - number of frames to operate over
// RETURN: 1 if the stopped before num_frames were returned. 0 otherwise.
int amg8833_run(amg8833_context amg8833, int num_frames)
{
   amg8833->num_frames = num_frames;

   while (amg8833->frame_counter < amg8833->num_frames || amg8833->num_frames == AMG8833_NUMFRAMES_ALL)
   {
      usleep(100 * 1000);
      if (!amg8833->framedata_observer)
         return 1;
      amg8833_run_update_framedata_observer(amg8833->context_id);
      amg8833->frame_counter++;
   }
   return 0;
}

void amg8833_stop(amg8833_context amg8833, bool stop_framedata)
{
   if (stop_framedata)
   {
      amg8833->framedata_observer = AMG8833_NULL_CONTEXT;
   }
}

bool amg8833_interrupts_enabled(amg8833_context ctx)
{
   uint8_t int_control = i2c_reg_read_byte(ctx->i2c, AMG_REG_INTERRUPT_CONTROL);
   if (int_control & AMG_INT_ENABLE_BMASK)
      return true;
   return false;
}

int amg8833_interrupt_mode(amg8833_context ctx)
{
   uint8_t int_control = i2c_reg_read_byte(ctx->i2c, AMG_REG_INTERRUPT_CONTROL);
   return (int_control & AMG_INT_MODE_BMASK) >> AMG_INT_MODE_BSHIFT;
}

amg8833_id amg8833_context_to_id(amg8833_context ctx)
{
   return ctx->device_id;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void amg8833_run_update_framedata_observer(int context_id)
{
   amg8833_context ctx = &contexts[context_id];

   for (int row = 0; row < AMG8833_ARRAY_HEIGHT; row++)
   {
      i2c_reg_read_many(ctx->i2c,
                        AMG_REG_PIXEL_ROW_0 + (row * AMG8833_ARRAY_WIDTH * AMG_BYTES_PER_PIXEL),
                        &ctx->frame_buffer_raw[row * AMG8833_ARRAY_WIDTH * AMG_BYTES_PER_PIXEL],
                        AMG8833_ARRAY_WIDTH * AMG_BYTES_PER_PIXEL);
   }

   for (int row = 0; row < AMG8833_ARRAY_HEIGHT; row++)
   {
      for (int col = 0; col < AMG8833_ARRAY_WIDTH; col++)
      {
         int raw_lsb_index = (row * AMG8833_ARRAY_WIDTH * AMG_BYTES_PER_PIXEL) + (col * AMG_BYTES_PER_PIXEL);
         uint16_t raw = ctx->frame_buffer_raw[raw_lsb_index + 1] << 8 | ctx->frame_buffer_raw[raw_lsb_index];
         float converted = (float)(raw & AMG_RAWPIXEL_VALUE_BITMASK);
         converted *= raw & AMG_RAWPIXEL_SIGN_BITMASK ? -1.0 : 1.0;
         ctx->frame_buffer[raw_lsb_index / 2] = converted;
      }
   }

   if (ctx->framedata_observer)
      ctx->framedata_observer(ctx, ctx->frame_buffer);
}