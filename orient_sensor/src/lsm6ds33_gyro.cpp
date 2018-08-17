#include <stdio.h>
#include <math.h>

#include "i2c_interface.h"

#define DS33_DEVICE_NAME "StMicro LSM6DS33 6-Axis Gyro & Accel"

//0x6b
#define DS33_SA0_HIGH_ADDRESS 0b1101011
//0x6a
#define DS33_SA0_LOW_ADDRESS  0b1101010

#define TEST_REG_ERROR -1

#define DS33_WHO_ID   0x69
#define WHO_AM_I      0x0F
#define STATUS_REG    0x1E
#define CTRL1_XL      0x10
#define CTRL2_G       0x11
#define CTRL3_C       0x12
#define CTRL4_C       0x13
#define CTRL5_C       0x14
#define CTRL6_C       0x15
#define CTRL7_G       0x16
#define CTRL8_XL      0x17
#define CTRL9_XL      0x18
#define CTRL10_C      0x19

#define OUT_TEMP_L    0x20
#define OUT_TEMP_H    0x21
#define OUTX_L_G      0x22
#define OUTX_H_G      0x23
#define OUTY_L_G      0x24
#define OUTY_H_G      0x25
#define OUTZ_L_G      0x26
#define OUTZ_H_G      0x27
#define OUTX_L_XL     0x28
#define OUTX_H_XL     0x29
#define OUTY_L_XL     0x2A
#define OUTY_H_XL     0x2B
#define OUTZ_L_XL     0x2C
#define OUTZ_H_XL     0x2D

#define DEFAULT_BUS 6

#define DS33_NUM_AXIS_PER_SENSOR 3
#define DS33_SENSOR_BYTE_RESOLUTION 2
#define DS33_GYRO_READ_WIDTH (DS33_NUM_AXIS_PER_SENSOR * DS33_SENSOR_BYTE_RESOLUTION)
#define DS33_ACCEL_READ_WIDTH (DS33_NUM_AXIS_PER_SENSOR * DS33_SENSOR_BYTE_RESOLUTION)

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2


int ds33_configure_gyro(mraa_i2c_context i2c)
{
    int error_occurred = 0;

    int scale_245dps = 0;
    int enable = 1;
    int rate_104hz = 2;
    int filter_50hz = 3;

    error_occurred = ds33_cfg_gyro_himode(i2c, enable);
    if (!error_occurred)
        error_occurred = ds33_cfg_gyro_scale(i2c, scale_245dps);
    if (!error_occurred)
        error_occurred = ds33_cfg_gyro_data_rate(i2c, rate_104hz);
    if (!error_occurred)
        error_occurred = ds33_cfg_gyro_analog_filter(i2c, rate_50hz);
    if (!error_occurred)
        error_occurred = ds33_cfg_gyro_(i2c, rate_100hz);

    return error_occurred;
}

int ds33_read_gyro_ready(mraa_i2c_context i2c, int *ready)
{
    int error_occurred = 0;
    uint8_t reg_val;
    uint8_t gyro_mask = 0x02;

    *ready = 0;

    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = read_register_bytes(i2c, STATUS_REG, &reg_val, 1);
    if (!error_occurred)
    {
        if (reg_val & gyro_mask)
            *ready = 1;
    }

    return error_occurred;
}

int ds33_read_gyro(mraa_i2c_context i2c, float *read_buf)
{
    int error_occurred = 0;
    uint8_t gyro_samples[DS33_GYRO_READ_WIDTH];

    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = read_register_bytes(i2c, OUTX_L_G, gyro_samples, DS33_GYRO_READ_WIDTH);
    if (!error_occurred)
    {
      uint8_t xlg = 0;
      uint8_t xhg = 1;
      uint8_t ylg = 2;
      uint8_t yhg = 3;
      uint8_t zlg = 4;
      uint8_t zhg = 5;

      // combine high and low bytes
      read_buf[AXIS_X] = (float)((int16_t)gyro_samples[xhg] << 8 | gyro_samples[xlg]);
      read_buf[AXIS_Y] = (float)((int16_t)gyro_samples[yhg] << 8 | gyro_samples[ylg]);
      read_buf[AXIS_Z] = (float)((int16_t)gyro_samples[zhg] << 8 | gyro_samples[zlg]);

      // +/- 245 degrees / second range
      float gscale = 245.0;
      float sample_max_val = 32768.0;

      read_buf[AXIS_X] = (read_buf[AXIS_X] * gscale) / sample_max_val;
      read_buf[AXIS_Y] = (read_buf[AXIS_Y] * gscale) / sample_max_val;
      read_buf[AXIS_Z] = (read_buf[AXIS_Z] * gscale) / sample_max_val;

      //printf("ds33_read_gyro : gx=%5.2f    gy=%5.2f    gz=%5.2f\n", read_buf[AXIS_X], read_buf[AXIS_Y], read_buf[AXIS_Z]);
    }

    return error_occurred;
}

static int ds33_cfg_gyro_data_rate(mraa_i2c_context i2c, int rate_idx)
{
    const uint8_t register_id   = CTRL2_G;
    const uint8_t rate_off      = 0x00;
    const uint8_t rate_52hz     = 0x03 << 4;
    const uint8_t rate_104hz    = 0x04 << 4;
    const uint8_t rate_208hz    = 0x05 << 4;
    const uint8_t rate_416hz    = 0x06 << 4;
    const uint8_t rate_833hz    = 0x07 << 4;
    const uint8_t rate_1_66khz  = 0x08 << 4;
    //                       0         1          2           3           4           5           6
    const uint8_t index[] = {rate_off, rate_52hz, rate_104hz, rate_208hz, rate_416hz, rate_833hz, rate_1_66khz};

    int error_occurred = 0;
    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_or_register_byte(i2c, register_id, index[rate_idx]);

    return error_occurred;
}

static int ds33_cfg_gyro_scale(mraa_i2c_context i2c, int scale_idx)
{
    const uint8_t register_id   = CTRL2_G;
    const uint8_t scale_245dps  = 0x00 << 2;
    const uint8_t scale_500dps  = 0x01 << 2;
    const uint8_t scale_1000dps = 0x02 << 2;
    const uint8_t scale_2000dps = 0x03 << 2;
    //                       0             1             2              3
    const uint8_t index[] = {scale_245dps, scale_500dps, scale_1000dps, scale_2000dps};

    int error_occurred = 0;
    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_or_register_byte(i2c, register_id, index[scale_idx]);

    return error_occurred;
}

static int ds33_cfg_gyro_hicut_filter(mraa_i2c_context i2c, int mode)
{
    const uint8_t register_id   = CTRL7_G;
    const uint8_t mode_disable  = 0x00 << 6;
    const uint8_t mode_enable   = 0x01 << 6;
    const uint8_t cut_0_0081hz  = 0x00 << 4;
    const uint8_t cut_0_0324hz  = 0x01 << 4;
    const uint8_t cut_2_07hz    = 0x02 << 4;
    const uint8_t cut_16_32hz   = 0x03 << 4;
    //                       0             1                           2                           3                         4
    const uint8_t index[] = {mode_disable, mode_enable | cut_0_0081hz, mode_enable | cut_0_0324hz, mode_enable | cut_2_07hz, mode_enable | cut_16_32hz};

    int error_occurred = 0;
    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_or_register_byte(i2c, register_id, index[mode]);

    return error_occurred;
}

static int ds33_cfg_gyro_read_enable(mraa_i2c_context i2c, int enable)
{
    const uint8_t register_id   = CTRL10_C;
    const uint8_t mode_disable  = 0x00 << 3;
    const uint8_t mode_enable   = 0x07 << 3;
    //                       0             1
    const uint8_t index[] = {mode_disable, mode_enable};

    int error_occurred = 0;
    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_or_register_byte(i2c, register_id, index[enable]);

    return error_occurred;
}

static int ds33_cfg_gyro_func_enable(mraa_i2c_context i2c, int enable)
{
    const uint8_t register_id   = CTRL10_C;
    const uint8_t mode_disable  = 0x00 << 2;
    const uint8_t mode_enable   = 0x01 << 2;
    //                       0             1
    const uint8_t index[] = {mode_disable, mode_enable};

    int error_occurred = 0;
    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_or_register_byte(i2c, register_id, index[enable]);

    return error_occurred;
}

