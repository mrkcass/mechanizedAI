#include <stdio.h>
#include <unistd.h>
#include "bno055.h"
#include "somax.h"
#include "i2c_interface.h"


#define BNO_CAMD_I2C_BUS        1
#define BNO_CAMD_I2C_ADDRESS    0x29
#define BNO_FRAME_I2C_BUS       0
#define BNO_FRAME_I2C_ADDRESS   0x28

#define BNO_REG_PAGEID           0x00
#define BNO_REG_CHIPID           0x00
#define BNO_REG_ACCEL_REV_ID     0x01
#define BNO_REG_MAG_REV_ID       0x02
#define BNO_REG_GYRO_REV_ID      0x03
#define BNO_REG_SW_REV_ID_LSB    0x04
#define BNO_REG_SW_REV_ID_MSB    0x05
#define BNO_REG_BL_REV_ID        0x06

#define BNO_REG_PWR_MODE         0x3E
#define BNO_PWR_MODE_MASK        0x00000011
#define BNO_PWR_MODE_SHIFT       0
#define BNO_PWR_MODE_NORM        0x00
#define BNO_PWR_MODE_LOW         0x01
#define BNO_PWR_MODE_OFF         0x02

#define BNO_REG_OPR_MODE         0x3D
#define BNO_OPR_MODE_MASK        0b00001111
#define BNO_OPR_MODE_SHIFT       0
#define BNO_OPR_MODE_CONFIG      0x00
#define BNO_OPR_MODE_ACC         0x01
#define BNO_OPR_MODE_MAG         0x02
#define BNO_OPR_MODE_GYRO        0x03
#define BNO_OPR_MODE_ACCMAG      0x04
#define BNO_OPR_MODE_ACCGYRO     0x05
#define BNO_OPR_MODE_MAGGYRO     0x06
#define BNO_OPR_MODE_ACCMAGGYRO  0x07
#define BNO_OPR_MODE_IMU         0x08
#define BNO_OPR_MODE_COMPASS     0x09
#define BNO_OPR_MODE_M4G         0x0A
#define BNO_OPR_MODE_NDOF_NOFMC  0x0B
#define BNO_OPR_MODE_NDOF        0x0C

#define BNO_REG_UNITS_EULER         0x3B
#define BNO_UNITS_EULER_MASK        0b00000100
#define BNO_UNITS_EULER_SHIFT       2
#define BNO_UNITS_EULER_DEG         0b00000000
#define BNO_UNITS_EULER_RAD         0b00000100

#define BNO_REG_UNITS_TEMP          0x3B
#define BNO_UNITS_TEMP_MASK         0b00010000
#define BNO_UNITS_TEMP_SHIFT        4
#define BNO_UNITS_TEMP_CEL          0b00000000
#define BNO_UNITS_TEMP_FAR          0b00010000

#define BNO_REG_DATA_FORMAT         0x3B
#define BNO_DATA_FORMAT_MASK        0b10000000
#define BNO_DATA_FORMAT_SHIFT       7
#define BNO_DATA_FORMAT_CCINC       0b00000000
#define BNO_DATA_FORMAT_CCDEC       0b10000000

#define BNO_REG_EULER_HEADING_MSB   0x1B
#define BNO_REG_EULER_HEADING_LSB   0x1A
#define BNO_REG_EULER_PITCH_MSB     0x1D
#define BNO_REG_EULER_PITCH_LSB     0x1C
#define BNO_REG_EULER_ROLL_MSB      0x1F
#define BNO_REG_EULER_ROLL_LSB      0x1E

#define BNO_REG_QUATERION_W_LSB     0x20
#define BNO_REG_QUATERION_W_MSB     0x21
#define BNO_REG_QUATERION_X_LSB     0x22
#define BNO_REG_QUATERION_X_MSB     0x23
#define BNO_REG_QUATERION_Y_LSB     0x24
#define BNO_REG_QUATERION_Y_MSB     0x25
#define BNO_REG_QUATERION_Z_LSB     0x26
#define BNO_REG_QUATERION_Z_MSB     0x27

#define BNO_REG_MAGDATA_X_LSB       0x0E
#define BNO_REG_MAGDATA_X_MSB       0x0F
#define BNO_REG_MAGDATA_Y_LSB       0x10
#define BNO_REG_MAGDATA_Y_MSB       0x11
#define BNO_REG_MAGDATA_Z_LSB       0x12
#define BNO_REG_MAGDATA_Z_MSB       0x13

#define BNO_REG_SYS_TRIGGER         0x3F
#define BNO_TRIGGER_INT             0x00
#define BNO_TRIGGER_EXT             0x80
#define BNO_SYS_TRIGGER_RESET       0x20

#define BNO_REG_SYS_ERROR           0x3A
#define BNO_REG_SYS_ERROR_MASK      0b11111111
#define BNO_REG_SYS_ERROR_SHIFT     0

#define BNO_REG_AXIS_MAP            0x41
#define BNO_REG_AXISMAP_X_MASK      0b00000011
#define BNO_REG_AXISMAP_X_SHIFT     0
#define BNO_REG_AXISMAP_Y_MASK      0b00001100
#define BNO_REG_AXISMAP_Y_SHIFT     2
#define BNO_REG_AXISMAP_Z_MASK      0b00110000
#define BNO_REG_AXISMAP_Z_SHIFT     4

#define BNO_REG_AXIS_SIGN           0x42
#define BNO_REG_AXISSIGN_Z_MASK     0b00000001
#define BNO_REG_AXISSIGN_Z_SHIFT    0
#define BNO_REG_AXISSIGN_Y_MASK     0b00000010
#define BNO_REG_AXISSIGN_Y_SHIFT    1
#define BNO_REG_AXISSIGN_X_MASK     0b00000100
#define BNO_REG_AXISSIGN_X_SHIFT    2

#define BNO_REG_CALIB_STATUS        0x35
#define BNO_REG_CALSTATUS_MAG_MASK  0b00000011
#define BNO_REG_CALSTATUS_MAG_SHIFT 0
#define BNO_REG_CALSTATUS_ACC_MASK  0b00001100
#define BNO_REG_CALSTATUS_ACC_SHIFT 2
#define BNO_REG_CALSTATUS_GYR_MASK  0b00110000
#define BNO_REG_CALSTATUS_GYR_SHIFT 4
#define BNO_REG_CALSTATUS_SYS_MASK  0b11000000
#define BNO_REG_CALSTATUS_SYS_SHIFT 6

#define BNO_REG_CALIB_ACC_OFFSETX_LSB  0x55
#define BNO_REG_CALIB_ACC_OFFSETX_MSB  0x56
#define BNO_REG_CALIB_ACC_OFFSETY_LSB  0x58
#define BNO_REG_CALIB_ACC_OFFSETY_MSB  0x59
#define BNO_REG_CALIB_ACC_OFFSETZ_LSB  0x59
#define BNO_REG_CALIB_ACC_OFFSETZ_MSB  0x5A
#define BNO_CALIB_ACC_OFFSET_RANGE_2G  2000
#define BNO_CALIB_ACC_OFFSET_RANGE_4G  4000
#define BNO_CALIB_ACC_OFFSET_RANGE_8G  8000
#define BNO_CALIB_ACC_OFFSET_RANGE_16G 16000

#define BNO_REG_CALIB_MAG_OFFSETX_LSB  0x5B
#define BNO_REG_CALIB_MAG_OFFSETX_MSB  0x5C
#define BNO_REG_CALIB_MAG_OFFSETY_LSB  0x5D
#define BNO_REG_CALIB_MAG_OFFSETY_MSB  0x5E
#define BNO_REG_CALIB_MAG_OFFSETZ_LSB  0x5F
#define BNO_REG_CALIB_MAG_OFFSETZ_MSB  0x60
#define BNO_CALIB_MAG_OFFSET_RANGE     6400

#define BNO_REG_CALIB_GYR_OFFSETX_LSB 0x61
#define BNO_REG_CALIB_GYR_OFFSETX_MSB 0x62
#define BNO_REG_CALIB_GYR_OFFSETY_LSB 0x63
#define BNO_REG_CALIB_GYR_OFFSETY_MSB 0x64
#define BNO_REG_CALIB_GYR_OFFSETZ_LSB 0x65
#define BNO_REG_CALIB_GYR_OFFSETZ_MSB 0x66
#define BNO_CALIB_ACC_OFFSET_RANGE_2K  32000
#define BNO_CALIB_ACC_OFFSET_RANGE_1K  16000
#define BNO_CALIB_ACC_OFFSET_RANGE_500  8000
#define BNO_CALIB_ACC_OFFSET_RANGE_250  4000
#define BNO_CALIB_ACC_OFFSET_RANGE_125  2000

#define BNO_REG_CALIB_ACC_RADUIS_LSB  0x67
#define BNO_REG_CALIB_ACC_RADUIS_MSB  0x68
#define BNO_REG_CALIB_MAG_RADUIS_LSB  0x69
#define BNO_REG_CALIB_MAG_RADUIS_MSB  0x6A
#define BNO_REG_CALIB_ACC_RADUIS_LSB  0x67
#define BNO_REG_CALIB_ACC_RADUIS_MSB  0x68


#define BNO_REG_PAGE_ID    0x07

#define BNO_INIT 0

#define BNO_CHIP_ID  0xA0
#define BNO_ACC_ID   0xFB
#define BNO_MAG_ID   0x32
#define BNO_GYR_ID   0x0F

#define BNO_MAPAXIS_X  0x0
#define BNO_MAPAXIS_Y  0x1
#define BNO_MAPAXIS_Z  0x2

#define BNO_AXISSIGN_POSITIVE     0
#define BNO_AXISSIGN_NEGATIVE     1

#define BNO_CALSTATUS_UNCALIBRATED  0
#define BNO_CALSTATUS_WORKING1      1
#define BNO_CALSTATUS_WORKING2      2
#define BNO_CALSTATUS_CALIBRATED    3

#define SWITCH_TO_CONFIG_MS      7
#define SWITCH_FROM_CONFIG_MS    19



struct BNO_CONTEXT
{
   i2c_context i2c;
   AHRS_EULER_CALLBACK euler_observer;
   AHRS_QUATERNION_CALLBACK quaternion_observer;
   AHRS_MAGNETOMETER_CALLBACK magnetometer_observer;
   int axis_map;
   int axis_sign;
   smx_byte run_mode;
};


static struct BNO_CONTEXT contexts[BNO_NUM_DEVICES];
static const char pwr_mode_strings[][32] =
{
   "BNO_PWR_MODE_NORM",
   "BNO_PWR_MODE_LOW",
   "BNO_PWR_MODE_OFF",
};
static const char opr_mode_strings[][32] =
{
   "BNO_OPR_MODE_CONFIG",
   "BNO_OPR_MODE_ACC",
   "BNO_OPR_MODE_MAG",
   "BNO_OPR_MODE_GYRO",
   "BNO_OPR_MODE_ACCMAG",
   "BNO_OPR_MODE_ACCGYRO",
   "BNO_OPR_MODE_MAGGYRO",
   "BNO_OPR_MODE_ACCMAGGYRO",
   "BNO_OPR_MODE_IMU",
   "BNO_OPR_MODE_COMPASS",
   "BNO_OPR_MODE_M4G",
   "BNO_OPR_MODE_NDOF_NOFMC",
   "BNO_OPR_MODE_NDOF",
};
static const char euler_unit_strings[][32] =
{
   "Degrees",
   "Radians"
};
static const char temp_unit_strings[][32] =
{
   "Celcius",
   "Farenheit"
};
static const char data_format_strings[][32] =
{
   "Clock wise increasing",
   "Clock wise decreasing"
};
static const char syserr_strings[][64] =
{
   "No error",
   "Peripheral initialization error",
   "System initialization error",
   "Self test result failed",
   "Register map value out of range",
   "Register map address out of range",
   "Register map write error",
   "BNO low power mode not available for selected operation mode",
   "Accelerometer power mode not available",
   "Fusion algorithm configuration error",
};

static int i2c_busdevice[] = {BNO_CAMD_I2C_BUS, BNO_CAMD_I2C_ADDRESS, BNO_FRAME_I2C_BUS, BNO_FRAME_I2C_ADDRESS};

static void bno055_run_configure_device(int dev_id);
static void bno055_run_update_euler_observer(int dev_id);
static void bno055_run_update_quaternion_observer(int dev_id);
static void bno055_run_update_magnetometer_observer(int dev_id);



ahrs_context bno055_open(ahrs_id id)
{
   struct BNO_CONTEXT* ctx = &contexts[id];

   ctx->i2c = i2c_open(i2c_busdevice[id*2], i2c_busdevice[(id*2)+1]);
   ctx->axis_map = BNO_MAPAXIS_X << BNO_REG_AXISMAP_X_SHIFT |
                   BNO_MAPAXIS_Y << BNO_REG_AXISMAP_Y_SHIFT |
                   BNO_MAPAXIS_Z << BNO_REG_AXISMAP_Z_SHIFT;
   ctx->axis_sign = BNO_SIGN_POSITIVE << BNO_REG_AXISSIGN_X_SHIFT |
                    BNO_SIGN_POSITIVE << BNO_REG_AXISSIGN_Y_SHIFT |
                    BNO_SIGN_POSITIVE << BNO_REG_AXISSIGN_Z_SHIFT;
   return (ahrs_context)ctx;
}

void bno055_close()
{
}

void bno055_cfg_run_mode(ahrs_context ahrs, smx_byte mode)
{
   BNO_CONTEXT *ctx = (BNO_CONTEXT *)ahrs;
   ctx->run_mode = mode;
}

void bno055_configure_axis(ahrs_context ahrs, int axis, int mapped_axis, int mapped_sign)
{
   BNO_CONTEXT* ctx = (BNO_CONTEXT*)ahrs;
   int axmap[] = {BNO_AXIS_X, BNO_AXIS_Y, BNO_AXIS_Z};

   if (axis == BNO_AXIS_X)
   {
      ctx->axis_map &= ~BNO_REG_AXISMAP_X_MASK;
      ctx->axis_map |= axmap[mapped_axis] << BNO_REG_AXISMAP_X_SHIFT;
   }
   else if (axis == BNO_AXIS_Y)
   {
      ctx->axis_map &= ~BNO_REG_AXISMAP_Y_MASK;
      ctx->axis_map |= axmap[mapped_axis] << BNO_REG_AXISMAP_Y_SHIFT;
   }
   else if (axis == BNO_AXIS_Z)
   {
      ctx->axis_map &= ~BNO_REG_AXISMAP_Z_MASK;
      ctx->axis_map |= axmap[mapped_axis] << BNO_REG_AXISMAP_Z_SHIFT;
   }

   if (mapped_sign != BNO_SIGN_SAME)
   {
      if (axis == BNO_AXIS_X)
      {
         ctx->axis_sign &= ~BNO_REG_AXISSIGN_X_MASK;
         if (mapped_sign == BNO_SIGN_NEGATIVE)
            ctx->axis_sign |= BNO_AXISSIGN_NEGATIVE << BNO_REG_AXISMAP_X_SHIFT;
      }
      else if (axis == BNO_AXIS_Y)
      {
         ctx->axis_sign &= ~BNO_REG_AXISSIGN_Y_MASK;
         if (mapped_sign == BNO_SIGN_NEGATIVE)
            ctx->axis_sign |= BNO_AXISSIGN_NEGATIVE << BNO_REG_AXISMAP_Y_SHIFT;
      }
      else if (axis == BNO_AXIS_Z)
      {
         ctx->axis_sign &= ~BNO_REG_AXISSIGN_Z_MASK;
         if (mapped_sign == BNO_SIGN_NEGATIVE)
            ctx->axis_sign |= BNO_AXISSIGN_NEGATIVE << BNO_REG_AXISMAP_Z_SHIFT;
      }
   }
}

void bno055_calibration_status(ahrs_context ahrs, int *calibstat_sys, int *calibstat_gyr, int *calibstat_acc, int *calibstat_mag)
{
   BNO_CONTEXT *ctx = (BNO_CONTEXT *)ahrs;

   uint8_t status = i2c_reg_read_byte(ctx->i2c, BNO_REG_CALIB_STATUS);
   *calibstat_sys = (status & BNO_REG_CALSTATUS_SYS_MASK) >> BNO_REG_CALSTATUS_SYS_SHIFT;
   *calibstat_mag = (status & BNO_REG_CALSTATUS_MAG_MASK) >> BNO_REG_CALSTATUS_MAG_SHIFT;
   *calibstat_gyr = (status & BNO_REG_CALSTATUS_GYR_MASK) >> BNO_REG_CALSTATUS_GYR_SHIFT;
   *calibstat_acc = (status & BNO_REG_CALSTATUS_ACC_MASK) >> BNO_REG_CALSTATUS_ACC_SHIFT;
}

//if ahrs_context is NULL, then set callback for all devices.
void bno055_output_callbk_euler(ahrs_context ahrs, AHRS_EULER_CALLBACK euler_callbk)
{
   BNO_CONTEXT *ctx = (BNO_CONTEXT *)ahrs;

   if (ctx)
   {
      ctx->euler_observer = euler_callbk;
      return;
   }

   for (int i = 0; i < AHRS_NUM_DEVICES; i++)
   {
      bno055_output_callbk_euler((ahrs_context)&contexts[i], euler_callbk);
   }
}

//if ahrs_context is NULL, then set callback for all devices.
void bno055_output_callbk_quaternion(ahrs_context ahrs, AHRS_QUATERNION_CALLBACK quaternion_callbk)
{
   BNO_CONTEXT *ctx = (BNO_CONTEXT *)ahrs;

   if (ctx)
   {
      ctx->quaternion_observer = quaternion_callbk;
      return;
   }

   for (int i = 0; i < AHRS_NUM_DEVICES; i++)
   {
      bno055_output_callbk_quaternion((ahrs_context)&contexts[i], quaternion_callbk);
   }
}

//if ahrs_context is NULL, then set callback for all devices.
void bno055_output_callbk_magnetometer(ahrs_context ahrs, AHRS_MAGNETOMETER_CALLBACK magnetometer_callbk)
{
   BNO_CONTEXT *ctx = (BNO_CONTEXT *)ahrs;

   if (ctx)
   {
      ctx->magnetometer_observer = magnetometer_callbk;
      return;
   }

   for (int i = 0; i < AHRS_NUM_DEVICES; i++)
   {
      bno055_output_callbk_magnetometer((ahrs_context)&contexts[i], magnetometer_callbk);
   }
}

int bno055_run()
{
   for (int i = 0; i < AHRS_NUM_DEVICES; i++)
   {
      if (contexts[i].i2c)
      {
         if (!contexts[i].euler_observer && !contexts[i].quaternion_observer && !contexts[i].magnetometer_observer)
            somax_log_add(SOMAX_LOG_WARN, "BNO055-%d : No assigned data output callbacks");
         bno055_run_configure_device(i);
      }
   }

   while (1)
   {
      for(int i=0; i < AHRS_NUM_DEVICES; i++)
      {
         if (!contexts[i].i2c)
            continue;
         if (contexts[i].euler_observer)
            bno055_run_update_euler_observer(i);
         if (contexts[i].quaternion_observer)
            bno055_run_update_quaternion_observer(i);
         if (contexts[i].magnetometer_observer)
            bno055_run_update_magnetometer_observer(i);
      }
      usleep(125*1000);
   }
   return 1;
}

ahrs_id bno055_context_to_id(ahrs_context ahrs)
{
   for (int i=0; i < BNO_NUM_DEVICES; i++)
   {
      if ((ahrs_context)&contexts[i] == ahrs)
         return i;
   }
   return 0;
}

void bno055_info(ahrs_context ahrs)
{
   BNO_CONTEXT *ctx = (BNO_CONTEXT *)ahrs;

   i2c_dev_write_byte(ctx->i2c, BNO_INIT);
   int chip_id = i2c_dev_read_byte(ctx->i2c);
   printf("  ChipID           : 0x%X\n", chip_id);

   int accel_id = i2c_reg_read_byte(ctx->i2c, BNO_REG_ACCEL_REV_ID);
   printf("  Accelerometer Rev: 0x%X%s\n", accel_id,
          accel_id == BNO_ACC_ID ? "" : "\t\tWARNING -- IDMISMATCH");

   int mag_id = i2c_reg_read_byte(ctx->i2c, BNO_REG_MAG_REV_ID);
   printf("  Magnetometer Rev : 0x%X%s\n", mag_id,
          mag_id == BNO_MAG_ID ? "" : "\t\tWARNING -- IDMISMATCH");

   int gyro_id = i2c_reg_read_byte(ctx->i2c, BNO_REG_GYRO_REV_ID);
   printf("  Gyroscope Rev    : 0x%X%s\n", gyro_id,
          gyro_id == BNO_GYR_ID ? "" : "\t\tWARNING -- IDMISMATCH");

   int sw_rev_lsb = i2c_reg_read_byte(ctx->i2c, BNO_REG_SW_REV_ID_LSB);
   int sw_rev_msb = i2c_reg_read_byte(ctx->i2c, BNO_REG_SW_REV_ID_MSB);
   int sw_rev = sw_rev_msb << 8 | sw_rev_lsb;
   printf("  Software Rev     : 0x%X\n", sw_rev);

   int bootl_rev = i2c_reg_read_byte(ctx->i2c, BNO_REG_BL_REV_ID);
   printf("  Software Rev     : 0x%X\n", bootl_rev);

   int pwr_mode = i2c_reg_read_byte(ctx->i2c, BNO_REG_PWR_MODE);
   pwr_mode = (pwr_mode & BNO_PWR_MODE_MASK) >> BNO_PWR_MODE_SHIFT;
   printf("  Power Mode       : [0x%X]%s\n", pwr_mode, pwr_mode_strings[pwr_mode]);

   int opr_mode = i2c_reg_read_byte(ctx->i2c, BNO_REG_OPR_MODE);
   opr_mode = (opr_mode & BNO_OPR_MODE_MASK) >> BNO_OPR_MODE_SHIFT;
   printf("  Operation Mode   : [0x%X]%s\n", opr_mode, opr_mode_strings[opr_mode]);

   int regval = i2c_reg_read_byte(ctx->i2c, BNO_REG_UNITS_EULER);
   regval = (regval & BNO_UNITS_EULER_MASK) >> BNO_UNITS_EULER_SHIFT;
   printf("  Euler units      : [0x%X]%s\n", regval, euler_unit_strings[regval]);

   regval = i2c_reg_read_byte(ctx->i2c, BNO_REG_UNITS_TEMP);
   regval = (regval & BNO_UNITS_TEMP_MASK) >> BNO_UNITS_TEMP_SHIFT;
   printf("  Temperature units: [0x%X]%s\n", regval, temp_unit_strings[regval]);

   regval = i2c_reg_read_byte(ctx->i2c, BNO_REG_DATA_FORMAT);
   regval = (regval & BNO_DATA_FORMAT_MASK) >> BNO_DATA_FORMAT_SHIFT;
   printf("  Data Format      : [0x%X]%s\n", regval, data_format_strings[regval]);
}

static void bno055_run_configure_device(int dev_id)
{
   printf("Starting BNO055 device id %d\n", dev_id);

   i2c_reg_write_byte(contexts[dev_id].i2c, BNO_REG_OPR_MODE, BNO_OPR_MODE_CONFIG);
   usleep(SWITCH_FROM_CONFIG_MS * U_MILLISECOND);

   i2c_reg_write_byte(contexts[dev_id].i2c, BNO_REG_SYS_TRIGGER, BNO_SYS_TRIGGER_RESET);
   usleep(1000 * U_MILLISECOND);

   uint8_t detected_chip_id = i2c_dev_read_byte(contexts[dev_id].i2c);
   while (detected_chip_id != BNO_CHIP_ID)
   {
      usleep(SWITCH_FROM_CONFIG_MS * U_MILLISECOND);
      detected_chip_id = i2c_dev_read_byte(contexts[dev_id].i2c);
   }
   usleep(100 * U_MILLISECOND);

   i2c_reg_write_byte(contexts[dev_id].i2c, BNO_REG_PWR_MODE, BNO_PWR_MODE_NORM);
   usleep(SWITCH_FROM_CONFIG_MS * U_MILLISECOND);

   i2c_reg_write_byte(contexts[dev_id].i2c, BNO_REG_PAGEID, 0);

   i2c_reg_write_byte(contexts[dev_id].i2c, BNO_REG_SYS_TRIGGER, 0);
   usleep(SWITCH_FROM_CONFIG_MS * 10 * U_MILLISECOND);

   i2c_reg_write_byte(contexts[dev_id].i2c, BNO_REG_AXIS_MAP, contexts[dev_id].axis_map);
   i2c_reg_write_byte(contexts[dev_id].i2c, BNO_REG_AXIS_SIGN, contexts[dev_id].axis_sign);

   if (contexts[dev_id].run_mode == BNO_RUNMODE_ACCMAGGYRO)
      i2c_reg_write_byte(contexts[dev_id].i2c, BNO_REG_OPR_MODE, BNO_OPR_MODE_ACCMAGGYRO);
   else
      i2c_reg_write_byte(contexts[dev_id].i2c, BNO_REG_OPR_MODE, BNO_OPR_MODE_NDOF);

   usleep(SWITCH_FROM_CONFIG_MS * 20 * U_MILLISECOND);

   int regval = i2c_reg_read_byte(contexts[dev_id].i2c, BNO_REG_SYS_ERROR);
   regval = (regval & BNO_REG_SYS_ERROR_MASK) >> BNO_REG_SYS_ERROR_SHIFT;
}

static void bno055_run_update_euler_observer(int dev_id)
{
   uint8_t bytes_msb_lsb[2];
   i2c_reg_read_many(contexts[dev_id].i2c, BNO_REG_EULER_HEADING_LSB, bytes_msb_lsb, 2);
   int16_t value = (((int16_t)bytes_msb_lsb[1]) << 8) | ((int16_t)bytes_msb_lsb[0]);
   float heading = (float)value / 16.0;

   i2c_reg_read_many(contexts[dev_id].i2c, BNO_REG_EULER_PITCH_LSB, bytes_msb_lsb, 2);
   value = (((int16_t)bytes_msb_lsb[1]) << 8) | ((int16_t)bytes_msb_lsb[0]);
   float pitch = (float)value / 16.0;

   i2c_reg_read_many(contexts[dev_id].i2c, BNO_REG_EULER_ROLL_LSB, bytes_msb_lsb, 2);
   value = (((int16_t)bytes_msb_lsb[1]) << 8) | ((int16_t)bytes_msb_lsb[0]);
   float roll = (float)value / 16.0;

   contexts[dev_id].euler_observer((ahrs_context)&contexts[dev_id], heading, pitch, roll);
}

static void bno055_run_update_quaternion_observer(int dev_id)
{
   uint8_t bytes_msb_lsb[8];
   i2c_reg_read_many(contexts[dev_id].i2c, BNO_REG_QUATERION_W_LSB, &bytes_msb_lsb[0], 8);
   int16_t value = (((int16_t)bytes_msb_lsb[1]) << 8) | ((int16_t)bytes_msb_lsb[0]);
   float w = (float)(value) / 16384.0;

   //i2c_reg_read_many(contexts[dev_id].i2c, BNO_REG_QUATERION_X_LSB, bytes_msb_lsb, 2);
   value = (((int16_t)bytes_msb_lsb[3]) << 8) | ((int16_t)bytes_msb_lsb[2]);
   float x = (float)(value) / 16384.0;

   //i2c_reg_read_many(contexts[dev_id].i2c, BNO_REG_QUATERION_Y_LSB, bytes_msb_lsb, 2);
   value = (((int16_t)bytes_msb_lsb[5]) << 8) | ((int16_t)bytes_msb_lsb[4]);
   float y = (float)(value) / 16384.0;

   //i2c_reg_read_many(contexts[dev_id].i2c, BNO_REG_QUATERION_Z_LSB, bytes_msb_lsb, 2);
   value = (((int16_t)bytes_msb_lsb[7]) << 8) | ((int16_t)bytes_msb_lsb[6]);
   float z = (float)(value) / 16384.0;

   contexts[dev_id].quaternion_observer((ahrs_context)&contexts[dev_id], w, x, y, z);
}

static void bno055_run_update_magnetometer_observer(int dev_id)
{
   uint8_t bytes_msb_lsb[6];
   i2c_reg_read_many(contexts[dev_id].i2c, BNO_REG_MAGDATA_X_LSB, &bytes_msb_lsb[0], 6);
   int16_t x = (((int16_t)bytes_msb_lsb[1]) << 8) | ((int16_t)bytes_msb_lsb[0]);
   int16_t y = (((int16_t)bytes_msb_lsb[3]) << 8) | ((int16_t)bytes_msb_lsb[2]);
   int16_t z = (((int16_t)bytes_msb_lsb[5]) << 8) | ((int16_t)bytes_msb_lsb[4]);

   contexts[dev_id].magnetometer_observer((ahrs_context)&contexts[dev_id], x, y, z);
}
