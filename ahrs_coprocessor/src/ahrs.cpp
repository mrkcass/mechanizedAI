#include "ahrs.h"
#include "bno055.h"

static void ahrs_test_callback(int device_id, int axis, int value);

static const char ahrs_names[][32] =
   {"AHRS-NULL", "AHRS-CAMD[BNO055]", "AHRS-FRAME[BNO055]"};


ahrs_context ahrs_open_i2c(int ahrs_id, i2c_context i2c, int i2c_address)
{
   return bno055_open(ahrs_id, i2c, i2c_address);
}

void ahrs_close()
{
   bno055_close();
}

void ahrs_info(ahrs_context ahrs)
{
   printf ("AHRS chip info: %s -------------\n", ahrs_names[ahrs]);
   bno055_info(ahrs);
}

int ahrs_test()
{
   printf("starting test: \n");
   ahrs_run(ahrs_test_callback);
   return 1;
}

int ahrs_run(AHRS_CALLBACK call_on_change)
{
   bno055_run(call_on_change);
   return 1;
}

static void ahrs_test_callback(int device_id, int axis, int value)
{
   static int current[AHRS_NUM_DEVICES + 1][AHRS_NUM_AXIS];

   if (value < 0)
      value = (360 * 100) + value;

   current[device_id][axis] = value;

   printf("FPAN[%3d.%2d] FTILT[%3d.%2d] FROTATE[%3d.%2d] CPAN[%3d.%2d] CTILT[%3d.%2d] CROTATE[%3d.%2d]\r",
          current[2][0] / 100, current[2][0] - ((current[2][0] / 100) * 100),
          current[2][1] / 100, current[2][1] - ((current[2][1] / 100) * 100),
          current[2][2] / 100, current[2][2] - ((current[2][2] / 100) * 100),
          current[1][0] / 100, current[1][0] - ((current[1][0] / 100) * 100),
          current[1][1] / 100, current[1][1] - ((current[1][1] / 100) * 100),
          current[1][2] / 100, current[1][2] - ((current[1][2] / 100) * 100));
   fflush(stdout);
}