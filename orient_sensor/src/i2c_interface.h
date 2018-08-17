#ifndef __i2c_interface_h__
#define __i2c_interface_h__

#include "mraa.h"
#include "mraa/i2c.h"

int latch_device(mraa_i2c_context i2c, uint8_t device_id);
int write_byte(mraa_i2c_context i2c, uint8_t byte);
int write_register_byte(mraa_i2c_context i2c, uint8_t register_id, uint8_t value);
int write_or_register_byte(mraa_i2c_context i2c, uint8_t register_id, uint8_t value);
int read_byte(mraa_i2c_context i2c, uint8_t * byte);
int read_register_bytes(mraa_i2c_context i2c, uint8_t register_id, uint8_t * buffer, int num_bytes);

void debug(int argc, char ** argv);

#endif