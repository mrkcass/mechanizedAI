//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: i2c_interface.cpp
// DESCRIPTION: Interface for I2C bus communication.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <stdio.h>

#include "mraa.h"
#include "mraa/i2c.h"

#include "somax.h"
#include "i2c_interface.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define TEST_REG_ERROR -1

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA STRUCTURES
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct I2C_CONTEXT
{
   int bus_id;
   int device_id;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static char frequency_str[][32] = {
   "FREQUENCY_100KHZ",
   "FREQUENCY_400KHZ",
   "FREQUENCY_3400KHZ"
};

static int i2c_bus_map[SOMAX_NUM_MAINBOARDS+1][NUM_I2C_BUSES] =
{
   {0, 0}, //unknown
   {1, 6}, //edison
   {0, 0}  //hikey960
};
static int i2c_bus_context_count[NUM_I2C_BUSES] = {0, 0};
static mraa_i2c_context i2c_bus[NUM_I2C_BUSES];
static struct I2C_CONTEXT contexts[MAX_I2C_CONTEXTS];

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void i2c_detect_devices(int bus);
static mraa_result_t i2c_get(int bus, uint8_t device_address, uint8_t register_address, uint8_t* data);
static int i2c_latch_device(i2c_context i2c);

bool i2c_detect_device(int bus, int device_address)
{
   uint8_t value;
   if (i2c_get(bus, device_address, 0, &value) == MRAA_SUCCESS)
      return true;
   return false;
}

i2c_context i2c_open(int i2c_bus_num, int device_address)
{
   if (i2c_bus[i2c_bus_num] == 0)
      i2c_bus[i2c_bus_num] = mraa_i2c_init(i2c_bus_map[somax_mainboard_id()][i2c_bus_num]);

   if (i2c_bus[i2c_bus_num] == NULL)
   {
      printf("\n Error: bus [%d] did not initialize\n", i2c_bus_num);
      return NULL;
   }

   int open_context_idx = MAX_I2C_CONTEXTS;
   for (int i=0; i < MAX_I2C_CONTEXTS; i++)
   {
      if (contexts[i].bus_id == 0 && contexts[i].device_id == 0)
      {
         open_context_idx = i;
         break;
      }
   }

   if (open_context_idx == MAX_I2C_CONTEXTS)
   {
      printf("\n Error creating i2c_context. I2C context limit reached\n");
      return NULL;
   }

   contexts[open_context_idx].bus_id = i2c_bus_num;
   contexts[open_context_idx].device_id = device_address;

   return &contexts[open_context_idx];
}

void i2c_close(i2c_context i2c)
{
   if (i2c == NULL)
      return;

   if (i2c->device_id)
   {
      i2c_bus_context_count[i2c->bus_id]--;
      if (i2c_bus_context_count[i2c->bus_id] == 0)
         mraa_i2c_stop(i2c_bus[i2c->bus_id]);
   }

   i2c->bus_id = i2c->device_id = 0;
}

int i2c_set_frequency(i2c_context i2c, int i2c_frequency_id)
{
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];
   mraa_result_t result = MRAA_SUCCESS;
    switch (i2c_frequency_id)
    {
    case I2C_FREQUENCY_100KHZ:
      result = mraa_i2c_frequency(bus, MRAA_I2C_STD);
      break;
    case I2C_FREQUENCY_400KHZ:
      result = mraa_i2c_frequency(bus, MRAA_I2C_FAST);
      break;
    case I2C_FREQUENCY_3400KHZ:
      result = mraa_i2c_frequency(bus, MRAA_I2C_HIGH);
      break;
    }

   if (result != MRAA_SUCCESS)
   {
      printf("Warning: i2c_interface set_frequency failed: %s\n", frequency_str[i2c_frequency_id]);
      return I2C_FREQUENCY_100KHZ;
   }
   printf("i2c_interface[I2C-%d]: frequency set to %s\n", i2c->bus_id, frequency_str[i2c_frequency_id]);
   return i2c_frequency_id;
}

//returns -1 when the bus id is not known, the mainboard bus id otherwise.
int i2c_somax_bus_id_to_mainboard_id(int somax_i2c_busid)
{
   if (somax_i2c_busid < 0 || somax_i2c_busid >= NUM_I2C_BUSES)
   {
      somax_log_add(SOMAX_LOG_ERR, "i2c mainboard bus lookup out of range: %d", somax_i2c_busid);
      return -1;
   }

   return i2c_bus_map[somax_mainboard_id()][somax_i2c_busid];
}

int i2c_dev_read_byte(i2c_context i2c)
{
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);
   int byte_read = mraa_i2c_read_byte(bus);
   if (byte_read == -1)
   {
      printf("Error: i2c couldn't read device byte\n");
   }

   return byte_read;
}

int i2c_reg16_read_byte(i2c_context i2c, uint16_t address)
{
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);

   uint8_t bytes[2];
   bytes[0] = address >> 8;
   bytes[1] = address & 0xFF;
   int write_status = mraa_i2c_write(bus, bytes, 2);
   if (write_status != MRAA_SUCCESS)
   {
      printf("Error: i2c couldn't write device bytes\n");
   }

   int byte_read = mraa_i2c_read_byte(bus);
   if (byte_read == -1)
   {
      printf("Error: i2c couldn't read device byte\n");
   }

   return byte_read;
}

int i2c_dev_read_word(i2c_context i2c)
{
   uint8_t read_buff[2];
   int num_bytes_read;
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);
   num_bytes_read = mraa_i2c_read(bus, read_buff, 2);
   if (num_bytes_read == -1)
   {
      printf("Error: i2c couldn't read device word\n");
   }

   return read_buff[0] << 8 | read_buff[1];
}

int i2c_reg16_read_word(i2c_context i2c, uint16_t address)
{
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);

   uint8_t bytes[2];
   bytes[0] = address >> 8;
   bytes[1] = address & 0xFF;
   int write_status = mraa_i2c_write(bus, bytes, 2);
   if (write_status != MRAA_SUCCESS)
   {
      printf("Error: i2c couldn't write device bytes\n");
   }

   int msb = mraa_i2c_read_byte(bus);
   int lsb = mraa_i2c_read_byte(bus);
   if (msb == -1 || lsb == -1)
   {
      printf("Error: i2c couldn't read device byte\n");
   }

   return ((msb & 0xFF) << 8) | (lsb & 0xFF);
}

int i2c_dev_read_bytes(i2c_context i2c, uint8_t *buffer, int num_bytes)
{
   int num_bytes_read;
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);
   num_bytes_read = mraa_i2c_read(bus, buffer, num_bytes);
   if (num_bytes_read == -1)
   {
      printf("Error: i2c couldn't read device bytes\n");
   }

   return num_bytes_read;
}

int i2c_dev_write_byte(i2c_context i2c, uint8_t byte)
{
   int error_occurred = 0;
   int write_status;
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);
   write_status = mraa_i2c_write_byte(bus, byte);
   if ( write_status != MRAA_SUCCESS)
   {
      printf("Error: i2c couldn't write device byte\n");
   }

   return error_occurred;
}

int i2c_dev_write_2bytes(i2c_context i2c, uint8_t byte0, uint8_t byte1)
{
   int write_status;
   uint8_t bytes[2];

   bytes[0] = byte0;
   bytes[1] = byte1;
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);
   write_status = mraa_i2c_write(bus, bytes, 2);
   if (write_status != MRAA_SUCCESS)
   {
      printf("Error: i2c couldn't write device bytes\n");
   }

   return write_status;
}

int i2c_reg16_write_byte(i2c_context i2c, uint16_t address, uint8_t byte)
{
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);

   uint8_t bytes[3];
   bytes[0] = (address >> 8) & 0xFF;
   bytes[1] = address & 0xFF;
   bytes[2] = byte;
   int write_status = mraa_i2c_write(bus, bytes, 3);
   if (write_status != MRAA_SUCCESS)
   {
      printf("Error: i2c couldn't write device bytes\n");
   }

   // int write_status = mraa_i2c_write_byte(bus, bytes[0]);
   // if (write_status != MRAA_SUCCESS)
   // {
   //    printf("Error: i2c couldn't write device bytes\n");
   // }

   // write_status = mraa_i2c_write(bus, bytes, 3);
   // if (write_status != MRAA_SUCCESS)
   // {
   //    printf("Error: i2c couldn't write device bytes\n");
   // }

   // write_status = mraa_i2c_write_byte(bus, byte);
   // if (write_status != MRAA_SUCCESS)
   // {
   //    printf("Error: i2c couldn't write device bytes\n");
   // }

   return write_status;
}

int i2c_reg16_write_2bytes(i2c_context i2c, uint16_t address, uint8_t byte0, uint8_t byte1)
{
   uint8_t bytes[2];

   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);

   bytes[0] = address >> 8;
   bytes[1] = address & 0xFF;
   int write_status = mraa_i2c_write(bus, bytes, 2);
   if (write_status != MRAA_SUCCESS)
   {
      printf("Error: i2c couldn't write device bytes\n");
   }

   bytes[0] = byte0;
   bytes[1] = byte1;
   write_status = mraa_i2c_write(bus, bytes, 2);
   if (write_status != MRAA_SUCCESS)
   {
      printf("Error: i2c couldn't write device bytes\n");
   }

   return write_status;
}

int i2c_dev_write_bytes(i2c_context i2c, uint8_t *bytes, int num_bytes)
{
   int write_status;
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);
   write_status = mraa_i2c_write(bus, bytes, num_bytes);
   if (write_status != MRAA_SUCCESS)
   {
      printf("Error: i2c couldn't write device bytes\n");
   }

   return write_status;
}

int i2c_reg16_write_bytes(i2c_context i2c, uint16_t address, uint8_t *bytes, int num_bytes)
{
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);
   uint8_t buff[2];
   buff[0] = address >> 8;
   buff[1] = address & 0xFF;
   int write_status = mraa_i2c_write(bus, buff, 2);
   if (write_status != MRAA_SUCCESS)
   {
      printf("Error: i2c couldn't write device bytes\n");
   }

   write_status = mraa_i2c_write(bus, bytes, num_bytes);
   if (write_status != MRAA_SUCCESS)
   {
      printf("Error: i2c couldn't write device bytes\n");
   }

   return write_status;
}



int i2c_reg_read_byte(i2c_context i2c, uint8_t register_id)
{
   int num_bytes_read;
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);
   num_bytes_read = mraa_i2c_read_byte_data(bus, register_id);
   if (num_bytes_read == -1)
   {
      printf("Error: i2c couldn't read register byte\n");
   }

   return num_bytes_read;
}

int i2c_reg_read_word(i2c_context i2c, uint8_t register_id)
{
   int num_bytes_read;
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);
   num_bytes_read = mraa_i2c_read_word_data(bus, register_id);
   if (num_bytes_read == -1)
   {
      printf("Error: i2c couldn't read register word\n");
   }

   return num_bytes_read;
}

int i2c_reg_read_many(i2c_context i2c, uint8_t first_register_id, uint8_t *buffer, int num_registers)
{
   int error_occurred = 0;
   int num_bytes_read;
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);
   num_bytes_read = mraa_i2c_read_bytes_data(bus, first_register_id, buffer, num_registers);
   if (num_bytes_read == -1)
   {
      printf("Error: i2c couldn't read bulk registers\n");
      error_occurred = 1;
   }

   return error_occurred;
}

int i2c_reg_write_byte(i2c_context i2c, uint8_t register_id, uint8_t value)
{
   mraa_result_t status = MRAA_SUCCESS;
   int error_occurred = 0;
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);
   status = mraa_i2c_write_byte_data(bus, value, register_id);
   if (status != MRAA_SUCCESS)
   {
      printf("Error: i2c couldn't write register byte (id=%x) value: %x\n", register_id, value);
      error_occurred = 1;
   }

   return error_occurred;
}

int i2c_reg_write_bits(i2c_context i2c, uint8_t register_id, uint8_t bit0, uint8_t num_bits, uint8_t bits)
{
   mraa_result_t status = MRAA_SUCCESS;
   int error_occurred = 0;
   uint8_t reg_val = 0;
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);
   reg_val = mraa_i2c_read_byte_data(bus, register_id);
   if (reg_val == -1)
   {
      printf("Error: i2c couldn't read byte");
      error_occurred = 1;
   }
   uint8_t mask = 1;
   for (int i=0; i < num_bits - 1; i++) mask *= 2;
   reg_val = (reg_val & !mask) + (bits << bit0);
   if (!error_occurred)
      status = mraa_i2c_write_byte_data(bus, reg_val, register_id);
   if (!error_occurred && status != MRAA_SUCCESS)
   {
      printf("Error: i2c couldn't write register orbyte (id=%x) value: %x\n", register_id, reg_val);
      error_occurred = 1;
   }

   return error_occurred;
}

int i2c_reg_write_word(i2c_context i2c, uint8_t register_id, uint16_t value)
{
   mraa_result_t status = MRAA_SUCCESS;
   int error_occurred = 0;
   mraa_i2c_context bus = i2c_bus[i2c->bus_id];

   i2c_latch_device(i2c);
   status = mraa_i2c_write_word_data(bus, value, register_id);
   if (status != MRAA_SUCCESS)
   {
      printf("Error: i2c couldn't write register word (id=%x) value: %x\n", register_id, value);
      error_occurred = 1;
   }

   return error_occurred;
}

void debug(int argc, char **argv)
{
   int bus;

   if (argc == 2)
      bus = strtol(argv[1], NULL, 0);
   else
   {
      printf("\n error no bus number provided\n");
      exit(1);
   }

   mraa_init();

   mraa_i2c_context i2c;
   i2c = mraa_i2c_init(i2c_bus_map[somax_mainboard_id()][bus]);

   i2c_detect_devices(bus);

   if (i2c == NULL)
   {
      printf("\n Error: bus [%d] did not initialize\n", bus);
   }

   exit(0);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void i2c_detect_devices(int bus)
{
   int addr;
   for (addr = 0x0; addr < 0x80; ++addr)
   {
      uint8_t value;
      if ((addr) % 16 == 0)
         printf("%02x: ", addr);
      if (i2c_get(bus, addr, 0, &value) == MRAA_SUCCESS)
         printf("%02x ", addr);
      else
         printf("-- ");
      if ((addr + 1) % 16 == 0)
         printf("\n");
   }
}

static int i2c_latch_device(i2c_context i2c)
{
   mraa_result_t status = MRAA_SUCCESS;
   int error_occurred = 0;

   if (i2c == NULL)
      return 1;

   status = mraa_i2c_address(i2c_bus[i2c->bus_id], i2c->device_id);
   if (status != MRAA_SUCCESS)
   {
      printf("Error: i2c couldn't latch address:%x\n", i2c->device_id);
      error_occurred = 1;
   }

   return error_occurred;
}

static mraa_result_t i2c_get(int bus, uint8_t device_address, uint8_t register_address, uint8_t* data)
{
   mraa_result_t status = MRAA_SUCCESS;
   mraa_i2c_context i2c = mraa_i2c_init(i2c_bus_map[somax_mainboard_id()][bus]);
   if (i2c == NULL) {
      return MRAA_ERROR_NO_RESOURCES;
   }
   status = mraa_i2c_address(i2c, device_address);
   if (status != MRAA_SUCCESS) {
      goto i2c_get_exit;
   }
   status = mraa_i2c_write_byte(i2c, register_address);
   if (status != MRAA_SUCCESS) {
      goto i2c_get_exit;
   }
   status = mraa_i2c_read(i2c, data, 1) == 1 ? MRAA_SUCCESS : MRAA_ERROR_UNSPECIFIED;
   if (status != MRAA_SUCCESS) {
      goto i2c_get_exit;
   }
i2c_get_exit:
   mraa_i2c_stop(i2c);
   return status;
}

