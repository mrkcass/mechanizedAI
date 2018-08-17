#include <stdio.h>

#include "mraa.h"
#include "mraa/i2c.h"
#include "i2c_interface.h"
#include "lsm6ds33.h"
#include "lis3mdl.h"
#include "madgwick_ahrs.h"

#define I2C_BUS_NUM 6
#define FILTER_SAMPLES_PER_SECOND 50

static int ONE_MILLISECOND = 1000;
static int TWENTY_MILLISECONDS = (20 * ONE_MILLISECOND);

//static int ONE_SECOND = (1000 * ONE_MILLISECOND);

Madgwick madgwick_filter;

void filter_samples(float * sample_buf_accel, float * sample_buf_gyro, float * sample_buf_mag, int print);

int main(int argc, char **argv)
{
	int error_occurred = 0;
	int bus = I2C_BUS_NUM;
	mraa_i2c_context i2c;

	if (argc > 1)
		debug(argc, argv);

	mraa_init();

	i2c = mraa_i2c_init(bus);

	if (i2c == NULL)
	{
		printf("\n Error: bus [%d] did not initialize\n", bus);
		return -1;
	}

	error_occurred = ds33_init(i2c, bus);
    if (!error_occurred)
        error_occurred = lis3_init(i2c, bus);

    float sample_buf_accel[3];
    float sample_buf_gyro[3];
    float sample_buf_mag[3];
    int print_counter = 50;
    madgwick_filter.begin(FILTER_SAMPLES_PER_SECOND);
    while (!error_occurred)
    {
        if (!error_occurred)
            error_occurred = ds33_sample(i2c, sample_buf_accel, sample_buf_gyro);
        if (!error_occurred)
            error_occurred = lis3_sample(i2c, sample_buf_mag);
        print_counter--;
        if (!print_counter)
        {
    	   filter_samples(sample_buf_accel, sample_buf_gyro, sample_buf_mag, 1);
           print_counter = 50;
        }
        else
            filter_samples(sample_buf_accel, sample_buf_gyro, sample_buf_mag, 0);
        usleep(TWENTY_MILLISECONDS);
    }

 	mraa_i2c_stop(i2c);

	return error_occurred;
}

void filter_samples(float * sample_buf_accel, float * sample_buf_gyro, float * sample_buf_mag, int print)
{
    // Offsets applied to raw x/y/z values
    //float mag_offsets[3]            = { -2.20F, -5.53F, -26.34F };
    float mag_offsets[3]            = { -1.0f, -1.0f, -1.0f };

    // Soft iron error compensation matrix
    float mag_softiron_matrix[3][3] = { { 0.934, 0.005, 0.013 },
                                        { 0.005, 0.948, 0.012 },
                                        { 0.013, 0.012, 1.129 } };

    int x = 0;
    int y = 2;
    int z = 3;

    // Apply mag offset compensation (base values in uTesla)
    float xo = sample_buf_mag[x] - mag_offsets[0];
    float yo = sample_buf_mag[y] - mag_offsets[1];
    float zo = sample_buf_mag[z] - mag_offsets[2];

    // Apply mag soft iron error compensation
    float mx = xo * mag_softiron_matrix[0][0] + yo * mag_softiron_matrix[0][1] + zo * mag_softiron_matrix[0][2];
    float my = xo * mag_softiron_matrix[1][0] + yo * mag_softiron_matrix[1][1] + zo * mag_softiron_matrix[1][2];
    float mz = xo * mag_softiron_matrix[2][0] + yo * mag_softiron_matrix[2][1] + zo * mag_softiron_matrix[2][2];

    // Update the filter
    madgwick_filter.update(sample_buf_gyro[x], sample_buf_gyro[y], sample_buf_gyro[z],
                           sample_buf_accel[x], sample_buf_accel[y], sample_buf_accel[z],
                           sample_buf_mag[x], sample_buf_mag[y], sample_buf_mag[z]);
                           //mx, my, mz);

    // Print the orientation filter output
    float roll = madgwick_filter.getRoll();
    float pitch = madgwick_filter.getPitch();
    float heading = madgwick_filter.getYaw();

    if (print)
        printf ("heading: %5f pitch: %5f roll: %5f\n", heading, pitch, roll);
}

