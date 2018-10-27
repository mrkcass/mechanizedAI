#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mraa.h"
#include "somax.h"
#include "ahrs.h"


#define SOMAX_NAME "SOMAX-AHRSCAMD, SOMAX-AHRSFRAME"
#define SOMAX_CLASS "SOMAX_HARDWARE-INPUT-ORIENTATION"
#define SOMAX_VERSION "1.0.0"
#define SOMAX_RESOURCES "SOMAX-I2C1 SOMAX-I2C6"

static ahrs_context ahrs_camd;
static ahrs_context ahrs_frame;

int ahrscop_open_camd();
int ahrscop_open_frame();
void ahrscop_close();
void ahrscop_display_help();
void ahrscop_info();
void ahrscop_test();

void test_callback(ahrs_context, int axis, int value);



int main(int argc, char *argv[])
{
   somax_data_init(SOMAX_NAME, SOMAX_CLASS, SOMAX_VERSION, SOMAX_RESOURCES);

   if (argc > 1 && (somax_commandline_has_option(argc, argv, "help") ||
                    somax_commandline_has_option(argc, argv, "--help") ||
                    somax_commandline_has_option(argc, argv, "-h")))
   {
      ahrscop_display_help();
      return 1;
   }

   int return_code = 0;

   mraa_init();

   return_code = ahrscop_open_camd();

   if (!return_code)
      return_code = ahrscop_open_frame();

   if (!return_code && somax_commandline_has_option(argc, argv, "info"))
   {
      ahrscop_info();
   }
   else if (!return_code && somax_commandline_has_option(argc, argv, "test"))
   {
      ahrscop_info();
      ahrs_cfg_run_mode(ahrs_camd, AHRS_RUNMODE_AHRS);
      ahrs_cfg_run_mode(ahrs_frame, AHRS_RUNMODE_AHRS);
      ahrs_test(AHRS_OUTPUTFORMAT_QUATERNION);
   }
   else if (!return_code && somax_commandline_has_option(argc, argv, "test-mag"))
   {
      ahrs_cfg_run_mode(ahrs_camd, AHRS_RUNMODE_IMU);
      ahrs_cfg_run_mode(ahrs_frame, AHRS_RUNMODE_IMU);
      ahrscop_info();
      ahrs_test(AHRS_OUTPUTFORMAT_MAGNETOMETER);
   }
   else if (!return_code && somax_commandline_has_option(argc, argv, "run-ahrs"))
   {
      ahrs_cfg_run_mode(ahrs_camd, AHRS_RUNMODE_AHRS);
      ahrs_cfg_run_mode(ahrs_frame, AHRS_RUNMODE_AHRS);
      ahrs_test(AHRS_OUTPUTFORMAT_QUATERNION);
   }
   else if (!return_code && somax_commandline_has_option(argc, argv, "run-imu"))
   {
      ahrs_cfg_run_mode(ahrs_camd, AHRS_RUNMODE_IMU);
      ahrs_cfg_run_mode(ahrs_frame, AHRS_RUNMODE_IMU);
      ahrs_test(AHRS_OUTPUTFORMAT_MAGNETOMETER);
   }

   ahrscop_close();
   mraa_deinit();
   if (return_code >= 0)
      return 0;
   else
      return return_code;
}

void ahrscop_display_help()
{
   printf("\n");
   printf("  Somax Attitude, heading and reference system coprocessor driver\n");
   printf("  Version: %s\n", SOMAX_VERSION);
   printf("\n");
   printf("  commands:\n");
   printf("    info\n");
   printf("       Read and print ahrs device information\n");
   printf("    test\n");
   printf("       Intialize both AHRS and display heading, pitch, & yaw data to console.\n");
   printf("    test-mag\n");
   printf("       Intialize both AHRS and return magnetometer data to console.\n");
   printf("    run-ahrs\n");
   printf("       Initialize both AHRS and use coprcessor for more accurate readings\n");
   printf("       until control-c.\n");
   printf("    run-imu\n");
   printf("       Initialize both AHRS and return accelerometer, gyroscope and\n");
   printf("       magnetometer data until control-c.\n");
}

int ahrscop_open_camd()
{
   ahrs_camd = ahrs_open(AHRS_ID_CAMD);
   printf("ahrs_camd == %p\n", ahrs_camd);
   return ahrs_camd == AHRS_CONTEXT_NULL;
}

int ahrscop_open_frame()
{
   ahrs_frame = ahrs_open(AHRS_ID_FRAME);
   printf("ahrs_frame == %p\n", ahrs_frame);
   return ahrs_frame == AHRS_CONTEXT_NULL;
}

void ahrscop_close()
{
   ahrs_close();
}

void ahrscop_info()
{
   ahrs_info(ahrs_camd);
   ahrs_info(ahrs_frame);
}

void ahrscop_test()
{
   ahrs_test(AHRS_OUTPUTFORMAT_QUATERNION);
}

void test_callback(ahrs_context ahrs, int axis, int value)
{

}
