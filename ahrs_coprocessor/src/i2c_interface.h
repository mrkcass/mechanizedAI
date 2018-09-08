#ifndef __i2c_interface_h__
#define __i2c_interface_h__

#include "mraa.h"
#include "mraa/i2c.h"

typedef mraa_i2c_context i2c_context;

#define I2C_FREQUENCY_100KHZ MRAA_I2C_STD
#define I2C_FREQUENCY_400KHZ MRAA_I2C_FAST
#define I2C_FREQUENCY_3400KHZ MRAA_I2C_HIGH


i2c_context i2c_open(int i2c_bus_num);
void i2c_close (i2c_context i2c);

int i2c_set_frequency(i2c_context i2c, int i2c_frequency);

int i2c_latch_device(i2c_context i2c, uint8_t device_id);

int i2c_dev_read_byte(i2c_context i2c);
int i2c_dev_read_word(i2c_context i2c);
int i2c_dev_read_bytes(i2c_context i2c, uint8_t *bytes, int num_bytes);
int i2c_dev_write_byte(i2c_context i2c, uint8_t byte);
int i2c_dev_write_2bytes(i2c_context i2c, uint8_t byte0, uint8_t byte1);
int i2c_dev_write_bytes(i2c_context i2c, uint8_t *bytes, int num_bytes);



int i2c_reg_read_byte(i2c_context i2c, uint8_t register_id);
int i2c_reg_read_word(i2c_context i2c, u_int8_t register_id);
int i2c_reg_read_many(i2c_context i2c, uint8_t *bytes, int num_registers);
int i2c_reg_write_byte(i2c_context i2c, uint8_t register_id, uint8_t byte);
int i2c_reg_write_bits(i2c_context i2c, uint8_t register_id, uint8_t bit0, uint8_t num_bits, uint8_t bits);
int i2c_reg_write_word(i2c_context i2c, u_int8_t register_id, uint16_t word);




















// int write_or_byte(i2c_context i2c, uint8_t register_id, uint8_t value);

// int i2c_reg_write_byte(i2c_context i2c, uint8_t register_id, uint8_t value);
// int i2c_reg_write_orbyte(i2c_context i2c, uint8_t register_id, uint8_t value);

// int write_or_byte(i2c_context i2c, uint8_t register_id, uint8_t value);
// int read_byte(i2c_context i2c, uint8_t * byte);
// int read_bytes(i2c_context i2c, uint8_t register_id, uint8_t * buffer, int num_bytes);

void debug(int argc, char ** argv);

#endif