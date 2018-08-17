#include <stdio.h>

#include "i2c_interface.h"

#define LIS3_DEVICE_NAME "StMicro LIS3MDL 3-Axis Magnetometer"

#define LIS3_SA0_HIGH_ADDRESS  0b0011110
#define LIS3_SA1_LOW_ADDRESS   0b0011100

#define TEST_REG_ERROR -1

#define LIS3_WHO_ID  0x3D

#define WHO_AM_I    0x0F

#define CTRL_REG1   0x20
#define CTRL_REG2   0x21
#define CTRL_REG3   0x22
#define CTRL_REG4   0x23
#define CTRL_REG5   0x24

#define STATUS_REG  0x27
#define OUT_X_L     0x28
#define OUT_X_H     0x29
#define OUT_Y_L     0x2A
#define OUT_Y_H     0x2B
#define OUT_Z_L     0x2C
#define OUT_Z_H     0x2D
#define TEMP_OUT_L  0x2E
#define TEMP_OUT_H  0x2F
#define INT_CFG     0x30
#define INT_SRC     0x31
#define INT_THS_L   0x32
#define INT_THS_H   0x33

#define LIS3_NUM_AXIS_PER_SENSOR 3
#define LIS3_SENSOR_BYTE_RESOLUTION 2
#define LIS3_MAG_READ_WIDTH (LIS3_NUM_AXIS_PER_SENSOR * LIS3_SENSOR_BYTE_RESOLUTION)

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

static int lis3_verify_identity(mraa_i2c_context i2c);
static int lis3_enable(mraa_i2c_context i2c);
static int lis3_read_mag(mraa_i2c_context i2c, float * read_buf);


int lis3_init(mraa_i2c_context i2c, int bus)
{
	int error_occurred = 0;

	error_occurred = lis3_verify_identity(i2c);
	if (!error_occurred)
		error_occurred = lis3_enable(i2c);

	return error_occurred;
}

int lis3_sample(mraa_i2c_context i2c, float * sample_buf_mag)
{
	int error_occurred = 0;

	error_occurred = lis3_read_mag(i2c, sample_buf_mag);

	return error_occurred;
}

static int lis3_enable(mraa_i2c_context i2c)
{
	int error_occurred = 0;

	error_occurred = latch_device(i2c, LIS3_SA0_HIGH_ADDRESS);
	if (!error_occurred)
		error_occurred = write_register_byte(i2c, CTRL_REG1, 0x70);
	if (!error_occurred)
		error_occurred = write_register_byte(i2c, CTRL_REG2, 0x00);
	if (!error_occurred)
		error_occurred = write_register_byte(i2c, CTRL_REG3, 0x00);
	if (!error_occurred)
		error_occurred = write_register_byte(i2c, CTRL_REG4, 0x0C);

	if (!error_occurred)
		printf("LIS3: enabled\n");

	return error_occurred;
}

static int lis3_read_mag(mraa_i2c_context i2c, float * read_buf)
{
	int error_occurred = 0;
	uint8_t mag_samples[LIS3_MAG_READ_WIDTH];

	error_occurred = latch_device(i2c, LIS3_SA0_HIGH_ADDRESS);
	if (!error_occurred)
		error_occurred = read_register_bytes(i2c, OUT_X_L | 0x80, mag_samples, LIS3_MAG_READ_WIDTH);
	if (!error_occurred)
	{
	  uint8_t xlm = 0;
	  uint8_t xhm = 1;
	  uint8_t ylm = 2;
	  uint8_t yhm = 3;
	  uint8_t zlm = 4;
	  uint8_t zhm = 5;

	  // combine high and low bytes
	  read_buf[AXIS_X] = (float)(mag_samples[xhm] << 8 | mag_samples[xlm]);
	  read_buf[AXIS_Y] = (float)(mag_samples[yhm] << 8 | mag_samples[ylm]);
	  read_buf[AXIS_Z] = (float)(mag_samples[zhm] << 8 | mag_samples[zlm]);

	  // +/- 4 gauss range
      float gscale = 4.0;
      float sample_max_val = 32768.0;

      read_buf[AXIS_X] = (read_buf[AXIS_X] * gscale) / sample_max_val;
      read_buf[AXIS_Y] = (read_buf[AXIS_Y] * gscale) / sample_max_val;
      read_buf[AXIS_Z] = (read_buf[AXIS_Z] * gscale) / sample_max_val;

	  //printf("lis3_read_mag : mx=%5d    my=%5d    mz=%5d\n", read_buf[AXIS_X], read_buf[AXIS_Y], read_buf[AXIS_Z]);
	}

	return error_occurred;
}

static int lis3_verify_identity(mraa_i2c_context i2c)
{
	int error_occurred = 0;
	uint8_t rx_tx_buf[1];

	error_occurred = latch_device(i2c, LIS3_SA0_HIGH_ADDRESS);
	if (!error_occurred)
		error_occurred = write_byte(i2c, WHO_AM_I);
	if (!error_occurred)
		error_occurred = read_byte(i2c, rx_tx_buf);
	if (!error_occurred)
	{
    	if (rx_tx_buf[0] == LIS3_WHO_ID)
    		printf("i2c found device: %s\n", LIS3_DEVICE_NAME);
    	else
    	{
    		printf ("i2c device not found: %s\n", LIS3_DEVICE_NAME);
    		error_occurred = 1;
    	}
    }

    return error_occurred;
}

static int ds33_cfg_mag_opmode(mraa_i2c_context i2c, int mode)
{
    const uint8_t register_id   = CTRL_REG1;
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

static int lis3_cfg_mag_data_rate(mraa_i2c_context i2c, int rate_idx)
{
    const uint8_t register_id   = CTRL_REG1;
    const uint8_t rate_0_625    = 0x00 << 2;
    const uint8_t rate_1_25     = 0x03 << 2;
    const uint8_t rate_2_5      = 0x04 << 2;
    const uint8_t rate_5_0      = 0x05 << 2;
    const uint8_t rate_10_0     = 0x06 << 2;
    const uint8_t rate_20_0     = 0x07 << 2;
	const uint8_t rate_40_0     = 0x08 << 2;
	const uint8_t rate_80_0     = 0x08 << 2;
    //                       0         1          2           3           4           5           6
    const uint8_t index[] = {rate_off, rate_52hz, rate_104hz, rate_208hz, rate_416hz, rate_833hz, rate_1_66khz};

    int error_occurred = 0;
    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_or_register_byte(i2c, register_id, index[rate_idx]);

    return error_occurred;
}

static int ds33_cfg_mag_scale(mraa_i2c_context i2c, int scale_idx)
{
    const uint8_t register_id   = CTRL2_G;
    const uint8_t scale_4g  = 0x00 << 5;
    const uint8_t scale_8g  = 0x01 << 5;
    const uint8_t scale_12g = 0x02 << 5;
    const uint8_t scale_16  = 0x03 << 5;
    //                       0         1         2          3
    const uint8_t index[] = {scale_4g, scale_8g, scale_12g, scale_16g};

    int error_occurred = 0;
    error_occurred = latch_device(i2c, DS33_SA0_HIGH_ADDRESS);
    if (!error_occurred)
        error_occurred = write_or_register_byte(i2c, register_id, index[scale_idx]);

    return error_occurred;
}



