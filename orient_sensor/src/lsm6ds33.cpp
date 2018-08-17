#include <stdio.h>
#include <math.h>

#include "i2c_interface.h"

#define DS33_DEVICE_NAME "StMicro LSM6DS33 6-Axis Gyro & Accel"

//0x6b
#define DS33_SA0_HIGH_ADDRESS 0b1101011
//0x6a
#define DS33_SA0_LOW_ADDRESS  0b1101010

#define TEST_REG_ERROR -1

#define DS33_WHO_ID    	0x69
#define WHO_AM_I 		0x0F
#define STATUS_REG      0x1E
#define CTRL1_XL 		0x10
#define CTRL2_G 		0x11
#define CTRL3_C 		0x12
#define CTRL4_C 		0x13
#define CTRL5_C 		0x14
#define CTRL6_C 		0x15
#define CTRL7_G 		0x16
#define CTRL8_XL 		0x17
#define CTRL9_XL 		0x18
#define CTRL10_C 		0x19

#define OUT_TEMP_L 	0x20
#define OUT_TEMP_H	0x21
#define OUTX_L_G  	0x22
#define OUTX_H_G  	0x23
#define OUTY_L_G  	0x24
#define OUTY_H_G  	0x25
#define OUTZ_L_G  	0x26
#define OUTZ_H_G  	0x27
#define OUTX_L_XL 	0x28
#define OUTX_H_XL 	0x29
#define OUTY_L_XL 	0x2A
#define OUTY_H_XL 	0x2B
#define OUTZ_L_XL 	0x2C
#define OUTZ_H_XL 	0x2D

#define DEFAULT_BUS 6

#define DS33_NUM_AXIS_PER_SENSOR 3
#define DS33_SENSOR_BYTE_RESOLUTION 2
#define DS33_GYRO_READ_WIDTH (DS33_NUM_AXIS_PER_SENSOR * DS33_SENSOR_BYTE_RESOLUTION)
#define DS33_ACCEL_READ_WIDTH (DS33_NUM_AXIS_PER_SENSOR * DS33_SENSOR_BYTE_RESOLUTION)

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2




extern int ds33_read_accel_ready(mraa_i2c_context i2c, int * ready);
extern int ds33_read_accel(mraa_i2c_context i2c, float * read_buf);

extern int ds33_read_gyro_ready(mraa_i2c_context i2c, int * ready);
extern int ds33_read_gyro(mraa_i2c_context i2c, float * read_buf);


static void normalize(float *a);
static int ds33_verify_identity(mraa_i2c_context i2c);
static int ds33_configure(mraa_i2c_context i2c);
static int ds33_enable(mraa_i2c_context i2c);

int ds33_init(mraa_i2c_context i2c, int bus)
{
    int error_occurred = 0;

    error_occurred = ds33_verify_identity(i2c);
    if (!error_occurred)
        error_occurred = ds33_configure(i2c);
    if (!error_occurred)
        error_occurred = ds33_enable(i2c);

    return error_occurred;
}

int ds33_sample(mraa_i2c_context i2c, float * sample_buff_accel, float * sample_buff_gyro)
{
    int error_occurred = 0;
    int data_ready = 0;
    int FIVEHUNDRED_MICROSECOND = 500;
    int retries = 0;
    int max_retries = 50;

    while (!data_ready)
    {
        if (!retries)
            usleep(FIVEHUNDRED_MICROSECOND);
        error_occurred = ds33_read_gyro_ready(i2c, &data_ready);
        if (error_occurred || data_ready)
            break;
        retries++;
        if (retries > max_retries)
            error_occurred = 1;
    }

    if (!error_occurred)
        error_occurred = ds33_read_accel(i2c, sample_buff_accel);
    if (!error_occurred)
        error_occurred = ds33_read_gyro(i2c, sample_buff_gyro);
    if (!error_occurred)
    {
        normalize(sample_buff_gyro);
    }

    return error_occurred;
}

static int ds33_configure(mraa_i2c_context i2c)
{
    int error_occurred = 0;

    int mode_continuous = 0;
    int auto_inc_enable = 1;

    error_occurred = ds33_cfg_accel(i2c);
    if (!error_occurred)
        error_occurred = ds33_cfg_gyro(i2c);
    if (!error_occurred)
        error_occurred = ds33_cfg_block_data(i2c, mode_continuous);
    if (!error_occurred)
        error_occurred = ds33_cfg_auto_increment(i2c, auto_inc_enable);

    return error_occurred;
}

static int ds33_enable(mraa_i2c_context i2c)
{
    int error_occurred = 0;

    return error_occurred;
}

static int ds33_verify_identity(mraa_i2c_context i2c)
{
    int error_occurred = 0;
    uint8_t rx_tx_buf[1];

    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_byte(i2c, WHO_AM_I);
    if (!error_occurred)
        error_occurred = read_byte(i2c, rx_tx_buf);
    if (!error_occurred)
    {
        if (rx_tx_buf[0] == DS33_WHO_ID)
            printf("i2c found device: %s\n", DS33_DEVICE_NAME);
        else
        {
            printf ("i2c device not found: %s\n", DS33_DEVICE_NAME);
            error_occurred = 1;
        }
    }

    return error_occurred;
}

static int ds33_cfg_block_data(mraa_i2c_context i2c, int mode)
{
    const uint8_t register_id           = CTRL7_G;
    const uint8_t mode_continuous       = 0x00 << 6;
    const uint8_t mode_register_read    = 0x01 << 6;
    //                       0                1
    const uint8_t index[] = {mode_continuous, mode_register_read};

    int error_occurred = 0;
    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_or_register_byte(i2c, register_id, index[mode]);

    return error_occurred;
}

static int ds33_cfg_auto_increment(mraa_i2c_context i2c, int enable)
{
    const uint8_t register_id   = CTRL7_G;
    const uint8_t disable_mode  = 0x00 << 2;
    const uint8_t enable_mode   = 0x01 << 2;
    //                       0        1
    const uint8_t index[] = {disable_mode, enable_mode};

    int error_occurred = 0;
    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_or_register_byte(i2c, register_id, index[enable]);

    return error_occurred;
}

static void vector_cross(float *a, float *b, float *out)
{
    static int x = 0;
    static int y = 1;
    static int z = 2;

    //out->x out[0] = (a->ya[1] * b->zb[2] ) - (a->za[2] * b->y);
    //out->y = (a->z * b->x) - (a->x * b->z);
    //out->z = (a->x * b->y) - (a->y * b->x);

    out[x] = (a[y] * b[z]) - (a[z] * b[y]);
    out[y] = (a[z] * b[x]) - (a[x] * b[z]);
    out[z] = (a[x] * b[y]) - (a[y] * b[x]);
}

static float dot_product(float *a, float *b)
{
    static int x = 0;
    static int y = 1;
    static int z = 2;

    return (a[x] * b[x]) + (a[y] * b[y]) + (a[z] * b[z]);
}

static void normalize(float *a)
{
    static int x = 0;
    static int y = 1;
    static int z = 2;

    float mag = sqrt(dot_product(a, a));

    a[x] /= mag;
    a[y] /= mag;
    a[z] /= mag;
}