#ifndef __i2c_interface_h__
#define __i2c_interface_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: i2c_interface.h
// DESCRIPTION: Interface for I2C bus communication.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include <stdint.h>

#define NUM_I2C_BUSES   2
#define I2C_BUSID_0     0
#define I2C_BUSID_1     1

#define MAX_I2C_CONTEXTS 32

#define I2C_FREQUENCY_100KHZ  0
#define I2C_FREQUENCY_400KHZ  1
#define I2C_FREQUENCY_3400KHZ 2

struct I2C_CONTEXT;

typedef I2C_CONTEXT* i2c_context;

i2c_context i2c_open(int i2c_bus_num, int device_address);
void i2c_close (i2c_context i2c);

int i2c_set_frequency(i2c_context i2c, int i2c_frequency_id);

int i2c_dev_read_byte(i2c_context i2c);
int i2c_dev_read_word(i2c_context i2c);
int i2c_dev_read_bytes(i2c_context i2c, uint8_t *bytes, int num_bytes);
int i2c_dev_write_byte(i2c_context i2c, uint8_t byte);
int i2c_dev_write_2bytes(i2c_context i2c, uint8_t byte0, uint8_t byte1);
int i2c_dev_write_bytes(i2c_context i2c, uint8_t *bytes, int num_bytes);



int i2c_reg_read_byte(i2c_context i2c, uint8_t register_id);
int i2c_reg_read_word(i2c_context i2c, uint8_t register_id);
int i2c_reg_read_many(i2c_context i2c, uint8_t first_register_id, uint8_t *buffer, int num_registers);
int i2c_reg_write_byte(i2c_context i2c, uint8_t register_id, uint8_t byte);
int i2c_reg_write_bits(i2c_context i2c, uint8_t register_id, uint8_t bit0, uint8_t num_bits, uint8_t bits);
int i2c_reg_write_word(i2c_context i2c, uint8_t register_id, uint16_t word);

int i2c_somax_bus_id_to_mainboard_id(int somax_i2c_busid);


void debug(int argc, char ** argv);

#endif