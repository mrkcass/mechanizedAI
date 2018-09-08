#ifndef __bno055_h__
#define __bno055_h__

#include "ahrs.h"
#include "i2c_interface.h"

#define BNO_DEVICE_CAMD    1
#define BNO_DEVICE_FRAME   2

ahrs_context bno055_open(int device_id, i2c_context i2c_interface, int i2c_address);
void bno055_close();
void bno055_info(ahrs_context);

#endif