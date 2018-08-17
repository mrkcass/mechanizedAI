#include <stdio.h>
#include <math.h>

#include "i2c_interface.h"

#define DS33_DEVICE_NAME "StMicro LSM6DS33 6-Axis Gyro & Accel"

//0x6b
#define DS33_SA0_HIGH_ADDRESS 0b1101011
//0x6a
#define DS33_SA0_LOW_ADDRESS  0b1101010

#define TEST_REG_ERROR -1

#define DS33_WHO_ID     0x69
#define WHO_AM_I        0x0F
#define STATUS_REG      0x1E
#define CTRL1_XL        0x10
#define CTRL2_G         0x11
#define CTRL3_C         0x12
#define CTRL4_C         0x13
#define CTRL5_C         0x14
#define CTRL6_C         0x15
#define CTRL7_G         0x16
#define CTRL8_XL        0x17
#define CTRL9_XL        0x18
#define CTRL10_C        0x19
#define TAP_CFG         0x58

#define OUT_TEMP_L      0x20
#define OUT_TEMP_H      0x21
#define OUTX_L_G        0x22
#define OUTX_H_G        0x23
#define OUTY_L_G        0x24
#define OUTY_H_G        0x25
#define OUTZ_L_G        0x26
#define OUTZ_H_G        0x27
#define OUTX_L_XL       0x28
#define OUTX_H_XL       0x29
#define OUTY_L_XL       0x2A
#define OUTY_H_XL       0x2B
#define OUTZ_L_XL       0x2C
#define OUTZ_H_XL       0x2D

#define DEFAULT_BUS 6

#define DS33_NUM_AXIS_PER_SENSOR 3
#define DS33_SENSOR_BYTE_RESOLUTION 2
#define DS33_GYRO_READ_WIDTH (DS33_NUM_AXIS_PER_SENSOR * DS33_SENSOR_BYTE_RESOLUTION)
#define DS33_ACCEL_READ_WIDTH (DS33_NUM_AXIS_PER_SENSOR * DS33_SENSOR_BYTE_RESOLUTION)

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

int ds33_configure_accel(mraa_i2c_context i2c)
{
    int error_occurred = 0;
    return error_occurred;
}

int ds33_read_accel_ready(mraa_i2c_context i2c, int *ready)
{
    int error_occurred = 0;
    uint8_t reg_val;
    uint8_t accel_mask = 0x01;

    *ready = 0;

    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = read_register_bytes(i2c, STATUS_REG, &reg_val, 1);
    if (!error_occurred)
    {
        if (reg_val & accel_mask)
            *ready = 1;
    }

    return error_occurred;
}

int ds33_read_accel(mraa_i2c_context i2c, float *read_buf)
{
    int error_occurred = 0;
    uint8_t accel_samples[DS33_ACCEL_READ_WIDTH];

    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = read_register_bytes(i2c, OUTX_L_XL, accel_samples, DS33_ACCEL_READ_WIDTH);
    if (!error_occurred)
    {
        uint8_t xla = 0;
        uint8_t xha = 1;
        uint8_t yla = 2;
        uint8_t yha = 3;
        uint8_t zla = 4;
        uint8_t zha = 5;

        // combine high and low bytes
        read_buf[AXIS_X] = (float)((int16_t)accel_samples[xha] << 8 | accel_samples[xla]);
        read_buf[AXIS_Y] = (float)((int16_t)accel_samples[yha] << 8 | accel_samples[yla]);
        read_buf[AXIS_Z] = (float)((int16_t)accel_samples[zha] << 8 | accel_samples[zla]);

        // +/- 2g range
        float ascale = 2.0;
        float sample_max_val = 32768.0;

        read_buf[AXIS_X] = (read_buf[AXIS_X] * ascale) / sample_max_val;
        read_buf[AXIS_Y] = (read_buf[AXIS_Y] * ascale) / sample_max_val;
        read_buf[AXIS_Z] = (read_buf[AXIS_Z] * ascale) / sample_max_val;

        //printf("ds33_read_accel: ax=%5.2f    ay=%5.2f    az=%5.2f\n", read_buf[AXIS_X], read_buf[AXIS_Y], read_buf[AXIS_Z]);
    }

    return error_occurred;
}

static int ds33_cfg_accel_data_rate(mraa_i2c_context i2c, int rate_idx)
{
    const uint8_t register_id   = CTRL1_XL;
    const uint8_t rate_off      = 0x00;
    const uint8_t rate_52hz     = 0x03 << 4;
    const uint8_t rate_104hz    = 0x04 << 4;
    const uint8_t rate_208hz    = 0x05 << 4;
    const uint8_t rate_416hz    = 0x06 << 4;
    const uint8_t rate_833hz    = 0x07 << 4;
    const uint8_t rate_1_66khz  = 0x08 << 4;
    const uint8_t rate_3_33khz  = 0x09 << 4;
    const uint8_t rate_6_66khz  = 0x0A << 4;
    //                       0         1          2           3           4           5           6             7             8
    const uint8_t index[] = {rate_off, rate_52hz, rate_104hz, rate_208hz, rate_416hz, rate_833hz, rate_1_66khz, rate_3_33khz, rate_6_66khz};

    int error_occurred = 0;
    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_or_register_byte(i2c, register_id, index[rate_idx]);

    return error_occurred;
}

static int ds33_cfg_accel_scale(mraa_i2c_context i2c, int scale_idx)
{
    const uint8_t register_id   = CTRL1_XL;
    const uint8_t scale_g2      = 0x00 << 2;
    const uint8_t scale_g4      = 0x02 << 2;
    const uint8_t scale_g8      = 0x03 << 2;
    const uint8_t scale_g16     = 0x01 << 2;
    //                       0         1         2         3
    const uint8_t index[] = {scale_g2, scale_g4, scale_g8, scale_g16};

    int error_occurred = 0;

    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_or_register_byte(i2c, register_id, index[scale_idx]);

    return error_occurred;
}

static int ds33_cfg_accel_bandwidth_filter(mraa_i2c_context i2c, int scale_idx)
{
    const uint8_t register1_id  = CTRL4_C;
    const uint8_t f_enable      = 0x00 << 7;
    const uint8_t f_disable     = 0x01 << 7;
    const uint8_t register2_id  = CTRL1_XL;
    const uint8_t rate_400hz    = 0x00 << 0;
    const uint8_t rate_200hz    = 0x01 << 0;
    const uint8_t rate_100hz    = 0x02 << 0;
    const uint8_t rate_50hz     = 0x03 << 0;
    //                       0           1           2           3
    const uint8_t index[] = {rate_400hz, rate_200hz, rate_100hz, rate_50hz};

    int error_occurred = 0;

    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
    {
        if (scale_idx)
            error_occurred = write_or_register_byte(i2c, register1_id, f_enable);
        else
            error_occurred = write_or_register_byte(i2c, register1_id, f_disable);
    }

    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_or_register_byte(i2c, register2_id, index[scale_idx]);

    return error_occurred;
}

static int ds33_cfg_accel_himode(mraa_i2c_context i2c, int enable)
{
    const uint8_t register_id   = CTRL6_C;
    const uint8_t mode_disable  = 0x00 << 4;
    const uint8_t mode_enable   = 0x01 << 4;
    //                       0             1
    const uint8_t index[] = {mode_disable, mode_enable};

    int error_occurred = 0;
    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_or_register_byte(i2c, register_id, index[enable]);

    return error_occurred;
}

static int ds33_cfg_accel_hipass_filter(mraa_i2c_context i2c, int mode)
{
    const uint8_t register_id   = CTRL8_XL;
    const uint8_t odr_div_100   = 0x01 << 5;
    const uint8_t odr_div_9     = 0x02 << 5;
    const uint8_t odr_div_400   = 0x03 << 5;
    const uint8_t f_disable     = 0x00 << 2;
    const uint8_t f_enable      = 0x01 << 2;
    //                       0          1                       2                     3
    const uint8_t index[] = {f_disable, odr_div_100 | f_enable, odr_div_9 | f_enable, odr_div_400 | f_enable};

    int error_occurred = 0;
    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_or_register_byte(i2c, register_id, index[mode]);

    return error_occurred;
}

static int ds33_cfg_accel_lowpass_filter2(mraa_i2c_context i2c, int enable)
{
    const uint8_t register_id   = CTRL8_XL;
    const uint8_t mode_disable  = 0x00 << 7;
    const uint8_t mode_enable   = 0x01 << 7;
    //                       0             1
    const uint8_t index[] = {mode_disable, mode_enable};

    int error_occurred = 0;
    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_or_register_byte(i2c, register_id, index[enable]);

    return error_occurred;
}

static int ds33_cfg_accel_read_enable(mraa_i2c_context i2c, int enable)
{
    const uint8_t register_id   = CTRL9_XL;
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

static int ds33_cfg_accel_digital_filters(mraa_i2c_context i2c, int enable)
{
    const uint8_t register_id   = TAP_CFG;
    const uint8_t mode_disable  = 0x00 << 4;
    const uint8_t mode_enable   = 0x01 << 4;
    //                       0             1
    const uint8_t index[] = {mode_disable, mode_enable};

    int error_occurred = 0;
    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_or_register_byte(i2c, register_id, index[enable]);

    return error_occurred;
}





