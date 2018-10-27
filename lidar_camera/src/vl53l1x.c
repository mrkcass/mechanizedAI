//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: vl53l1x.h
// DESCRIPTION: Driver for ST VL53L1X LIDAR imaging camera.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "vl53l1x.h"
#include "somax.h"
#include "i2c_interface.h"
#include "SparkFun_VL53L1X_Arduino_Library.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//VL53L1X registers and configuration constants / data
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define L1X_REG_MODELID          0x010F
#define L1X_MODELID              0xEA

#define L1X_REG_TYPEID           0x0110
#define L1X_TYPEID               0xCC

#define L1X_REG_REVISIONID       0x0111
#define L1X_REVISIONID           0x10



#define L1X_BYTES_PER_PIXEL      2

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define VL53L1X_MAX_CONTEXTS 16


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA STRUCTURES
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct VL53L1X_DEVICE_PROPERTIES
{
   vl53l1x_id device_id;
   char str_device_id[MAX_STR_DEVICEID_LEN];
   int i2c_bus;
   int i2c_address;
};

struct VL53L1X_CONTEXT
{
   i2c_context i2c;
   int context_id;
   vl53l1x_id device_id;
   vl53l1x_framedata_callback framedata_observer;
   int framedata_observer_callbk_id;
   int num_frames;
   int frame_counter;
   uint8_t * frame_buffer_raw;
   float * frame_buffer;
   bool using_external_framebuffer;
   unsigned char output_units;
   VL53L1X * driver;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static struct VL53L1X_DEVICE_PROPERTIES vl53l1x_devices[VL53L1X_NUM_DEVICES] =
{
   {
      VL53L1X_DEVICEID_1,
      "VL53L1X_DEVICEID_1",
      I2C_BUSID_0,
      0x29,
   },
};

static struct VL53L1X_CONTEXT contexts[VL53L1X_MAX_CONTEXTS];
static int vl53l1x_num_contexts;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void vl53l1x_run_update_framedata_observer(int context_id);

vl53l1x_context vl53l1x_open(vl53l1x_id id)
{
   if (vl53l1x_num_contexts >= VL53L1X_MAX_CONTEXTS)
   {
      somax_log_add(SOMAX_LOG_ERR, "VL53L1X : Open failed. No free contexts slots.");
      return VL53L1X_NULL_CONTEXT;
   }

   if (id < 0 || id > VL53L1X_NUM_DEVICES)
   {
      somax_log_add(SOMAX_LOG_ERR, "VL53L1X : Open failed. Unknown device id %d.", id);
      return VL53L1X_NULL_CONTEXT;
   }

   struct VL53L1X_CONTEXT* ctx = &contexts[vl53l1x_num_contexts];
   ctx->context_id = vl53l1x_num_contexts;
   ctx->i2c = i2c_open(vl53l1x_devices[id].i2c_bus, vl53l1x_devices[id].i2c_address);
   ctx->device_id = id;

   i2c_set_frequency(ctx->i2c, I2C_FREQUENCY_400KHZ);

   usleep(100 * U_MILLISECOND);

   vl53l1x_num_contexts++;
   return ctx;
}

void vl53l1x_close(vl53l1x_context ctx)
{
   if (ctx == NULL)
      return;
   if (ctx->frame_buffer_raw)
      free (ctx->frame_buffer_raw);
   if (ctx->frame_buffer && !ctx->using_external_framebuffer)
      free(ctx->frame_buffer);
   memset(ctx, 0, sizeof(struct VL53L1X_CONTEXT));
   vl53l1x_num_contexts--;
}

extern int i2c_reg16_read_byte(i2c_context i2c, uint16_t address);
extern int i2c_reg16_read_word(i2c_context i2c, uint16_t address);
extern int i2c_reg16_write_byte(i2c_context i2c, uint16_t address, uint8_t byte);
void vl53l1x_info(vl53l1x_context ctx)
{
   somax_mainboard_id_t id = somax_mainboard_id();
   int somax_bus = vl53l1x_devices[ctx->device_id].i2c_bus;
   int main_bus = i2c_somax_bus_id_to_mainboard_id(somax_bus);
   int address = vl53l1x_devices[ctx->device_id].i2c_address;
   printf("   Somax I2C bus:     %d\n", somax_bus);
   printf("   %s I2C bus:    %d\n", somax_mainboard_id_to_string(id), main_bus);
   printf("   I2C address:       0x%X\n", address);

   // i2c_dev_write_2bytes(ctx->i2c, 0, 0);
   int chip_id = i2c_reg16_read_byte(ctx->i2c, 0x0001);
   if (chip_id)
       printf("   ChipID:            0x%X\n", chip_id);
    else
       printf("   ChipID:            not supported\n");

   //software reset
   // i2c_dev_write_2bytes(ctx->i2c, 0x0b, 0);
   // i2c_dev_write_byte(ctx->i2c, 0x0);
   // somax_sleep(U_MILLISECOND);
   // i2c_dev_write_2bytes(ctx->i2c, 0x0b, 0);
   // i2c_dev_write_byte(ctx->i2c, 0x1);
   //somax_sleep(U_MILLISECOND * 2000);

   i2c_reg16_write_byte(ctx->i2c, 0x00E0, 0x02);

   int val;

   //model number
   //i2c_dev_write_2bytes(ctx->i2c, 0x01, 0x0F);
   int modelid = i2c_reg16_read_byte(ctx->i2c, 0x010F); //i2c_dev_read_byte(ctx->i2c);//i2c_dev_read_byte(ctx->i2c);
   if (modelid == L1X_MODELID)
      printf("   ModelID:           0x%02X\n", modelid);
   else
      printf("   ModelID:           0x%02X (Invalid) \n", modelid);
   //module number
   //i2c_dev_write_2bytes(ctx->i2c, 0x01, 0x10);
   int moduleid = i2c_reg16_read_byte(ctx->i2c, 0x0110); //i2c_dev_read_byte(ctx->i2c);
   if (moduleid == L1X_TYPEID)
      printf("   ModuleID:          0x%02X\n", moduleid);
   else
      printf("   ModuleID           0x%02X (Invalid) \n", moduleid);
   //revision number
   //i2c_dev_write_2bytes(ctx->i2c, 0x01, 0x11);
   int revisionid = i2c_reg16_read_byte(ctx->i2c, 0x0111); //i2c_dev_read_byte(ctx->i2c);
   if (revisionid == L1X_REVISIONID)
      printf("   TypeID:            0x%02X\n", revisionid);
   else
      printf("   TypeID:            0x%02X (Invalid) \n", revisionid);

   val = i2c_reg16_read_byte(ctx->i2c, 0x0026);
   printf("   0x0026  Debug mode:        0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x0027);
   printf("   0x0027  Test mode:         0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x002C);
   printf("   0x002C  Host status:       0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x002D);
   printf("   0x002D  I2C cfg:           0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x002E);
   printf("   0x002E  I2C cfg extra:     0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x0030);
   printf("   0x0030  GPIO MUX:          0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x0031);
   printf("   0x0031  GPIO TIO:          0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x0032);
   printf("   0x0032  GPIO FIO status:   0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x0046);
   printf("   0x0046  Interupt cfg:      0x%02X\n", val);

   val = i2c_reg16_read_word(ctx->i2c, 0x0064);
   printf("   0x0064  Range sigma:       0x%04X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x0077);
   printf("   0x0077  System speed:      0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x00DB);
   printf("   0x00DB  Cold boot status:  0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x00E0);
   printf("   0x00E0  Power down:        0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x00E1);
   printf("   0x00E1  Overdrv AVDD:      0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x00E2);
   printf("   0x00E2  VDD 1V2:           0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x00E3);
   printf("   0x00E3  Slow oscilatr:     0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x00E4);
   printf("   0x00E4  Test mode status:  0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x00E5);
   printf("   0x00E5  Firmware status:   0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x00E6);
   printf("   0x00E6  Frmwre mode stat:  0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x00E7);
   printf("   0x00E7  Frmwre mode stat2: 0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x00FF);
   printf("   0x00FF  Interptmgr status: 0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x0101);
   printf("   0x0101  Power reset stat:  0x%02X\n", val);

   int firmware_status = i2c_reg16_read_byte(ctx->i2c, 0x00E6);
   printf("   0x00E6  Firmware Status:   0x%02X\n", firmware_status);

   int firmware_mode = i2c_reg16_read_byte(ctx->i2c, 0x00E7);
   printf("   0x00E7  Firmware Mode:     0x%02X\n", firmware_mode);

   int clock_cfg = i2c_reg16_read_byte(ctx->i2c, 0x04C4);
   printf("   0x04C4  Clock config:      0x%02X\n", clock_cfg);

   int range_device = i2c_reg16_read_byte(ctx->i2c, 0x0680);
   printf("   0x0680  Range device:      0x%02X\n", range_device);

   int range_revision = i2c_reg16_read_byte(ctx->i2c, 0x0681);
   printf("   0x0681  Range revision:    0x%02X\n", range_revision);

   val = i2c_reg16_read_byte(ctx->i2c, 0x0980);
   printf("   0x0980  Range Status:      0x%02X\n", val);

   int spi_mode = i2c_reg16_read_byte(ctx->i2c, 0x07BD);
   printf("   0x07BD  SPI mode:          0x%02X\n", spi_mode);

   val = i2c_reg16_read_byte(ctx->i2c, 0x0085);
   printf("   0x0085  Firmware Enable:   0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x0086);
   printf("   0x0086  Intrp clear:       0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x0087);
   printf("   0x0087  Start mode:        0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x0088);
   printf("   0x0088  Intrp status:      0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x000B);
   printf("   0x000B  VHV config:        0x%02X\n", val);

   val = i2c_reg16_read_byte(ctx->i2c, 0x000A);
   printf("   0x000A  VHV config offset: 0x%02X\n", val);
}

// Specifiy a function to be called when an updated pixel data is ready.
// PARAM: ctx - VL53L1X context.
// PARAM: callback - function to call when a new frame of pixels is ready
// PARAM: frame_buffer - a data buffer that will hold new frame pixel data. if not
//           provided, a buffer will be allocated using malloc which will be sent to
//           the observer by way of 'callbk'. any buffer provided, no matter the storage
//           class is the property of the caller and should be destroyed accordingly
//
//if callback bufffer is NULL, use the driver buffer.
void vl53l1x_cfg_output_callbk_framedata(vl53l1x_context ctx, vl53l1x_callback_id callback_id, vl53l1x_framedata_callback callbk, vl53l1x_framedata_buffer frame_buffer)
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
      ctx->frame_buffer = (float *)malloc(VL53L1X_ARRAY_SIZE * sizeof(float));
   }

   if (!ctx->frame_buffer_raw)
      ctx->frame_buffer_raw = (uint8_t *)malloc(VL53L1X_ARRAY_SIZE * L1X_BYTES_PER_PIXEL);
}

// blocking call that runs an observer update loop.
// PARAM: vl53l1x - VL53L1X context
// PARAM: num_frames - number of frames to operate over
// RETURN: 1 if stopped before num_frames were returned. 0 otherwise.
int vl53l1x_run(vl53l1x_context vl53l1x, int num_frames)
{
   vl53l1x->num_frames = num_frames;

   if (!vl53l1x->driver)
      vl53l1x->driver = new VL53L1X();

   vl53l1x->driver->begin(vl53l1x->i2c);

   while (vl53l1x->frame_counter < vl53l1x->num_frames || vl53l1x->num_frames == VL53L1X_NUMFRAMES_ALL)
   {
      usleep(100 * U_MILLISECOND);
      if (!vl53l1x->framedata_observer)
         return 1;
      vl53l1x_run_update_framedata_observer(vl53l1x->context_id);
      vl53l1x->frame_counter++;
   }
   return 0;
}

void vl53l1x_stop(vl53l1x_context vl53l1x, bool stop_framedata)
{
   if (stop_framedata)
   {
      vl53l1x->framedata_observer = VL53L1X_NULL_CONTEXT;
   }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void vl53l1x_run_update_framedata_observer(int context_id)
{
   vl53l1x_context ctx = &contexts[context_id];

   printf("debug 1\n");

   ctx->driver->startMeasurement();

   printf("debug 2\n");
   while(ctx->driver->newDataReady() == false)
      somax_sleep(5 * U_MILLISECOND);

   printf("debug 3\n");

   ctx->frame_buffer[0] = ctx->driver->getDistance();

   ctx->framedata_observer(ctx, ctx->framedata_observer_callbk_id, ctx->frame_buffer);
}