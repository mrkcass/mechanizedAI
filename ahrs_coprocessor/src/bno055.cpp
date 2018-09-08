#include "bno055.h"

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

#define BNO_REG_OPR_MODE         0x3E
#define BNO_OPR_MODE_MASK        0b00001111
#define BNO_OPR_MODE_SHIFT       0
#define BNO_OPR_MODE_CONFIG      0x00
#define BNO_OPR_MODE_ACC         0x01
#define BNO_OPR_MODE_MAG         0x02
#define BNO_OPR_MODE_GYRO        0x03
#define BNO_OPR_MODE_ACCMAG      0x04
#define BNO_OPR_MODE_ACCGYRO     0x05
#define BNO_OPR_MODE_ACCMAGGYRO  0x06
#define BNO_OPR_MODE_IMU         0x07
#define BNO_OPR_MODE_COMPASS     0x08
#define BNO_OPR_MODE_M4G         0x09
#define BNO_OPR_MODE_NDOF_NOFMC  0x0A
#define BNO_OPR_MODE_NDOF        0x0B

#define BNO_REG_UNITS_EULER      0x3B
#define BNO_UNITS_EULER_MASK     0b00000100
#define BNO_UNITS_EULER_SHIFT    2
#define BNO_UNITS_EULER_DEG      0b00000000
#define BNO_UNITS_EULER_RAD      0b00000100

#define BNO_REG_UNITS_TEMP       0x3B
#define BNO_UNITS_TEMP_MASK      0b00010000
#define BNO_UNITS_TEMP_SHIFT     4
#define BNO_UNITS_TEMP_CEL       0b00000000
#define BNO_UNITS_TEMP_FAR       0b00010000

#define BNO_REG_DATA_FORMAT      0x3B
#define BNO_DATA_FORMAT_MASK     0b10000000
#define BNO_DATA_FORMAT_SHIFT    7
#define BNO_DATA_FORMAT_CCINC    0b00000000
#define BNO_DATA_FORMAT_CCDEC    0b10000000

#define BNO_INIT 0

#define BNO_ACC_ID 0xFB
#define BNO_MAG_ID 0x32
#define BNO_GYR_ID 0x0F

#define SWITCH_TO_CONFIG_MS      7
#define SWITCH_FROM_CONFIG_MS    19

struct AHRS_CONTEXT
{
   i2c_context i2c;
   int i2c_addr;
};


static struct AHRS_CONTEXT contexts[AHRS_NUM_DEVICES];
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

ahrs_context bno055_open(int device_id, i2c_context i2c, int i2c_address)
{
   int context_id = device_id;
   contexts[context_id].i2c = i2c;
   contexts[context_id].i2c_addr = i2c_address;

   return context_id;
}

void bno055_close()
{

}

void bno055_info(ahrs_context ahrs)
{
   i2c_latch_device(contexts[ahrs].i2c, contexts[ahrs].i2c_addr);
   i2c_dev_write_byte(contexts[ahrs].i2c, BNO_INIT);
   int chip_id = i2c_dev_read_byte(contexts[ahrs].i2c);
   printf("  ChipID           : 0x%X\n", chip_id);

   i2c_latch_device(contexts[ahrs].i2c, contexts[ahrs].i2c_addr);
   int accel_id = i2c_reg_read_byte(contexts[ahrs].i2c, BNO_REG_ACCEL_REV_ID);
   printf("  Accelerometer Rev: 0x%X%s\n", accel_id,
          accel_id == BNO_ACC_ID ? "" : "\t\tWARNING -- IDMISMATCH");

   i2c_latch_device(contexts[ahrs].i2c, contexts[ahrs].i2c_addr);
   int mag_id = i2c_reg_read_byte(contexts[ahrs].i2c, BNO_REG_MAG_REV_ID);
   printf("  Magnetometer Rev : 0x%X%s\n", mag_id,
          mag_id == BNO_MAG_ID ? "" : "\t\tWARNING -- IDMISMATCH");

   i2c_latch_device(contexts[ahrs].i2c, contexts[ahrs].i2c_addr);
   int gyro_id = i2c_reg_read_byte(contexts[ahrs].i2c, BNO_REG_GYRO_REV_ID);
   printf("  Gyroscope Rev    : 0x%X%s\n", gyro_id,
          gyro_id == BNO_GYR_ID ? "" : "\t\tWARNING -- IDMISMATCH");

   i2c_latch_device(contexts[ahrs].i2c, contexts[ahrs].i2c_addr);
   int sw_rev_lsb = i2c_reg_read_byte(contexts[ahrs].i2c, BNO_REG_SW_REV_ID_LSB);
   int sw_rev_msb = i2c_reg_read_byte(contexts[ahrs].i2c, BNO_REG_SW_REV_ID_MSB);
   int sw_rev = sw_rev_msb << 8 | sw_rev_lsb;
   printf("  Software Rev     : 0x%X\n", sw_rev);

   i2c_latch_device(contexts[ahrs].i2c, contexts[ahrs].i2c_addr);
   int bootl_rev = i2c_reg_read_byte(contexts[ahrs].i2c, BNO_REG_BL_REV_ID);
   printf("  Software Rev     : 0x%X\n", bootl_rev);

   i2c_latch_device(contexts[ahrs].i2c, contexts[ahrs].i2c_addr);
   int pwr_mode = i2c_reg_read_byte(contexts[ahrs].i2c, BNO_REG_PWR_MODE);
   pwr_mode = (pwr_mode & BNO_PWR_MODE_MASK) >> BNO_PWR_MODE_SHIFT;
   printf("  Power Mode       : [0x%X]%s\n", pwr_mode, pwr_mode_strings[pwr_mode]);

   i2c_latch_device(contexts[ahrs].i2c, contexts[ahrs].i2c_addr);
   int opr_mode = i2c_reg_read_byte(contexts[ahrs].i2c, BNO_REG_OPR_MODE);
   opr_mode = (opr_mode & BNO_OPR_MODE_MASK) >> BNO_OPR_MODE_SHIFT;
   printf("  Operation Mode   : [0x%X]%s\n", opr_mode, opr_mode_strings[opr_mode]);

   i2c_latch_device(contexts[ahrs].i2c, contexts[ahrs].i2c_addr);
   int regval = i2c_reg_read_byte(contexts[ahrs].i2c, BNO_REG_UNITS_EULER);
   regval = (regval & BNO_UNITS_EULER_MASK) >> BNO_UNITS_EULER_SHIFT;
   printf("  Euler units      : [0x%X]%s\n", regval, euler_unit_strings[opr_mode]);

   i2c_latch_device(contexts[ahrs].i2c, contexts[ahrs].i2c_addr);
   regval = i2c_reg_read_byte(contexts[ahrs].i2c, BNO_REG_UNITS_TEMP);
   regval = (regval & BNO_UNITS_TEMP_MASK) >> BNO_UNITS_TEMP_SHIFT;
   printf("  Temperature units: [0x%X]%s\n", regval, temp_unit_strings[opr_mode]);

   i2c_latch_device(contexts[ahrs].i2c, contexts[ahrs].i2c_addr);
   regval = i2c_reg_read_byte(contexts[ahrs].i2c, BNO_REG_DATA_FORMAT);
   regval = (regval & BNO_DATA_FORMAT_MASK) >> BNO_DATA_FORMAT_SHIFT;
   printf("  Data Format      : [0x%X]%s\n", regval, data_format_strings[opr_mode]);
}
