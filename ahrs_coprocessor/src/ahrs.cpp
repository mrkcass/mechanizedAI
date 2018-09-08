#include "ahrs.h"
#include "bno055.h"

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

int ahrs_test(int ahrs_id)
{
   return 1;
}

int ahrs_run()
{
   return 1;
}