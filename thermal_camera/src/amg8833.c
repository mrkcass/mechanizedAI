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
#include "amg8833.h"
#include "somax.h"
#include "i2c_interface.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//AMG8833 registers
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
#define AMG_THERMRESISTORMSB_RESOLUTION   .0625

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
   amg8833_id device_id;
   //AHRS_EULER_CALLBACK euler_observer;
   //AHRS_QUATERNION_CALLBACK quaternion_observer;
   //AHRS_MAGNETOMETER_CALLBACK magnetometer_observer;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static struct AMG8833_DEVICE_PROPERTIES amg8833_devices[AMG8833_NUM_DEVICES] =
{
   {
      AMG8833_DEVICE_1,
      "AMG8833_DEVICE_1",
      I2C_BUSID_1,
      0x69,
   },
};

static struct AMG8833_CONTEXT contexts[AMG8833_MAX_CONTEXTS];
static const char pwr_mode_strings[][32] =
{
   "AMG8833_PWR_MODE_NORM",
   "AMG8833_PWR_MODE_STANBY",
   "AMG8833_PWR_MODE_SLEEP",
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
amg8833_context amg8833_open(amg8833_id id)
{
   struct AMG8833_CONTEXT* ctx = &contexts[id];

   ctx->i2c = i2c_open(amg8833_devices[id].i2c_bus, amg8833_devices[id].i2c_address);
   ctx->device_id = id;

   i2c_set_frequency(ctx->i2c, I2C_FREQUENCY_400KHZ);
   i2c_reg_write_byte(ctx->i2c, AMG_REG_RESET, AMG_RESET_HARD);
   usleep(10 * U_MILLISECOND);
   i2c_reg_write_byte(ctx->i2c, AMG_REG_POWER_CONTROL, AMG_POWER_MODE_NORMAL);
   usleep(10 * U_MILLISECOND);
   i2c_reg_write_byte(ctx->i2c, AMG_REG_POWER_CONTROL, AMG_POWER_MODE_SLEEP);
   usleep(100 * U_MILLISECOND);

   return ctx;
}

void amg8833_close(amg8833_context ctx)
{
   if (ctx == NULL)
      return;

   memset(ctx, 0, sizeof(struct AMG8833_CONTEXT));
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
   float ftemp = (float)temp * AMG_THERMRESISTORMSB_RESOLUTION;

   if (sign)
      return -ftemp;

   return ftemp;
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