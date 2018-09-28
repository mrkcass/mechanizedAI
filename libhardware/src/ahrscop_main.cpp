#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "somax.h"
#include "i2c_interface.h"
#include "ahrs.h"
#include "bno055.h"


#define SOMAX_NAME "SOMAX-AHRSCAMD, SOMAX-AHRSFRAME"
#define SOMAX_CLASS "SOMAX_HARDWARE-INPUT-ORIENTATION"
#define SOMAX_VERSION "1.0.0"
#define SOMAX_RESOURCES "SOMAX-I2C1 SOMAX-I2C6"

#define AHRS_CAMD_I2C_BUS        6
#define AHRS_CAMD_I2C_ADDRESS    0x29
#define AHRS_FRAME_I2C_BUS       1
#define AHRS_FRAME_I2C_ADDRESS   0x29

static i2c_context ahrs_i2c_bus_camd;
static i2c_context ahrs_i2c_bus_frame;

static ahrs_context ahrs_camd;
static ahrs_context ahrs_frame;

int ahrscop_open_camd();
int ahrscop_open_frame();
void ahrscop_close();
void ahrscop_display_help();
void ahrscop_info();
void ahrscop_test();

void test_callback(int device_id, int axis, int value);



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
      ahrscop_info();
   else if (!return_code && somax_commandline_has_option(argc, argv, "test"))
   {
      ahrscop_info();
      ahrs_test();
   }
   else if (!return_code && somax_commandline_has_option(argc, argv, "sample-pwr"))
      ahrs_run(test_callback);

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
   printf("    test-camd\n");
   printf("       Intialize the CAMD AHRS and display readings to stdout.\n");
   printf("    test-frame\n");
   printf("       Intialize the FRAME AHRS and display readings to stdout.\n");
   printf("    run\n");
   printf("       Initialize and collect data from CAMD & FRAME AHRS\n");
   printf("       until control-c.\n");
}

int ahrscop_open_camd()
{
   ahrs_i2c_bus_camd = i2c_open(AHRS_CAMD_I2C_BUS);

   if (ahrs_i2c_bus_camd)
      ahrs_camd = ahrs_open_i2c(AHRS_ID_CAMD, ahrs_i2c_bus_camd, AHRS_CAMD_I2C_ADDRESS);

   return ahrs_camd == AHRS_CONTEXT_NULL;
}

int ahrscop_open_frame()
{
   ahrs_i2c_bus_frame = i2c_open(AHRS_FRAME_I2C_BUS);

   if (ahrs_i2c_bus_frame)
      ahrs_frame = ahrs_open_i2c(AHRS_ID_FRAME, ahrs_i2c_bus_frame, AHRS_FRAME_I2C_ADDRESS);

   return ahrs_frame == AHRS_CONTEXT_NULL;
}

void ahrscop_close()
{
   ahrs_close();
   if (ahrs_i2c_bus_camd)
      i2c_close(ahrs_i2c_bus_camd);
   if (ahrs_i2c_bus_frame)
      i2c_close(ahrs_i2c_bus_frame);
}

void ahrscop_info()
{
   ahrs_info(ahrs_camd);
   ahrs_info(ahrs_frame);
}

void ahrscop_test()
{
   ahrs_test();
}

void test_callback(int device_id, int axis, int value)
{
   static int current[AHRS_NUM_DEVICES + 1][AHRS_NUM_AXIS];

   current[device_id][axis] = value;

//    printf("FPAN[%3d.%2d] FTILT[%3d.%2d] FROTATE[%3d.%2d] CPAN[%3d.%2d] CTILT[%3d.%2d] CROTATE[%3d.%2d]\r",
//           current[2][0] / 100, current[2][0] - ((current[2][0] / 100) * 100),
//           current[2][1] / 100, current[2][1] - ((current[2][1] / 100) * 100),
//           current[2][2] / 100, current[2][2] - ((current[2][2] / 100) * 100),
//           current[1][0] / 100, current[1][0] - ((current[1][0] / 100) * 100),
//           current[1][1] / 100, current[1][1] - ((current[1][1] / 100) * 100),
//           current[1][2] / 100, current[1][2] - ((current[1][2] / 100) * 100));
//    fflush(stdout);
}
