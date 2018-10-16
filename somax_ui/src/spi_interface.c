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
#include "mraa/spi.h"

#include "somax.h"
#include "spi_interface.h"

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
struct SPI_CONTEXT
{
    spi_busid bus_id;
    int chipselect;
};

struct SPI_BUS
{
   mraa_spi_context spi;
   int context_count;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static int spi_bus_map[SPI_NUM_BUSID+1] =
{
   //no matter the board, element 0 is always the same.
   0,
   //{somax bus id, mainboard bus id}
   #if SOMAX_MAINBOARDID == SOMAX_MAINBOARDID_EDISON
   1,
   #elif SOMAX_MAINBOARDID == SOMAX_MAINBOARDID_HIKEY960
   0,
   #endif
};
static struct SPI_CONTEXT spi_contexts[SPI_MAX_CONTEXTS];
static int spi_contexts_used;

static struct SPI_BUS spi_bus[SPI_NUM_BUSID+1];

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
extern char * decode_error(mraa_result_t code);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

mraa_spi_context spi_init_bus(spi_busid busid)
{
   mraa_result_t result;

   printf("\nInitalizing SPI..\n");
   mraa_spi_context spi = mraa_spi_init(spi_bus_map[busid]);
   printf("  SPI: MRAA context = %p\n", spi);
   int freq = SPI_FREQUENCY_DEFAULT_KHZ * 1000;
   result = mraa_spi_frequency(spi, freq);
   if (result == MRAA_SUCCESS)
      printf("  SPI: frequency = %d\n", freq);
   else
      printf("  SPI ERROR: Could not set frequency = %d. [%d]%s\n", freq, result, decode_error(result));
   result = mraa_spi_mode(spi, MRAA_SPI_MODE0);
   if (result == MRAA_SUCCESS)
      printf("  SPI: mode = 0\n");
   else
      printf("  SPI ERROR: Could not set mode = 0. [%d]%s\n", result, decode_error(result));
   //initialize the bus
   mraa_spi_write(spi, 0x00);

   result = mraa_spi_lsbmode(spi, 0);
   if (result == MRAA_SUCCESS)
      printf("  SPI: lsbmode = 0\n");
   else
      printf("  SPI ERROR: Could not set lsbmode = 0. [%d]%s\n", result, decode_error(result));
   result = mraa_spi_bit_per_word(spi, 8);
   if (result == MRAA_SUCCESS)
      printf("  SPI: bits per word = 8\n");
   else
      printf("  SPI ERROR: Could not set bits per word = 8. [%d]%s\n", result, decode_error(result));

   spi_bus[busid].spi = spi;
   spi_bus[busid].context_count++;
   return spi;
}

void spi_deinit_bus(spi_busid busid)
{

}

spi_context spi_open(spi_busid bus_id, int chip_select)
{
   if (spi_bus[bus_id].spi == 0)
      if (!spi_init_bus(bus_id))
         return 0;

   if (spi_contexts_used >= SPI_MAX_CONTEXTS)
   {
      somax_log_add(SOMAX_LOG_ERR, "SPI: Error creating spi_context. SPI context limit reached");
      return NULL;
   }

   spi_context new_ctx = NULL;
   for (int i=0; i < SPI_MAX_CONTEXTS; i++)
   {
      if (spi_contexts[i].bus_id == 0 && spi_contexts[i].chipselect == 0)
      {
         new_ctx = &spi_contexts[i];
         break;
      }
   }

   new_ctx->bus_id = bus_id;
   new_ctx->chipselect = chip_select;

   return new_ctx;
}

void spi_close(spi_context spi)
{
   if (spi == NULL)
      return;

   if (spi->bus_id)
   {
      spi_bus[spi->bus_id].context_count--;
      if (spi_bus[spi->bus_id].context_count == 0)
         spi_deinit_bus(spi->bus_id);
   }

   spi->bus_id = spi->chipselect = 0;
}

int spi_set_frequency(spi_context spi, int freq_khz)
{
   if (freq_khz < SPI_FREQUENCY_MIN_KHZ || freq_khz > SPI_FREQUENCY_MAX_KHZ)
   {
      somax_log_add(SOMAX_LOG_ERR, "SPI: set frequency. requested out of range %d khz", freq_khz);
   }

   mraa_result_t result = mraa_spi_frequency(spi_bus[spi->bus_id].spi, freq_khz * 1000);
    if (result != MRAA_SUCCESS)
   {
       somax_log_add(SOMAX_LOG_WARN, "SPI. set_frequency failed at %d khz", freq_khz);
       return 0;
   }
   somax_log_add(SOMAX_LOG_INFO, "SPI. bus %d. set_frequency to %d khz", spi->bus_id - SPI_BUSID_0, freq_khz);
   return freq_khz;
}

// void spi_cmd_write_byte(mraa_spi_context spi, struct Ssd1351Gpio gpio, uint8_t value)
// {
//    set_gpio(gpio, "dc", 0);
//    int status = mraa_spi_write(spi, value);
//    if (status < 0)
//       printf("  SB1351 Error: Send command failed [%x]\n", value);
//    set_gpio(gpio, "dc", 1);
// }

// void send_spi_data(mraa_spi_context spi, struct Ssd1351Gpio gpio, uint8_t value)
// {
//    //set_gpio(gpio, "cs", 0);
//    set_gpio(gpio, "dc", 1);
//    int status = mraa_spi_write(spi, value);
//    if (status < 0)
//       printf("  SB1351 Error: Send data failed [%x]\n", value);
//    set_gpio(gpio, "dc", 0);
// }

// //returns -1 when the bus id is not known, the mainboard bus id otherwise.
// int i2c_somax_bus_id_to_mainboard_id(int somax_i2c_busid)
// {
//    if (somax_i2c_busid < 0 || somax_i2c_busid >= NUM_I2C_BUSES)
//    {
//       somax_log_add(SOMAX_LOG_ERR, "i2c mainboard bus lookup out of range: %d", somax_i2c_busid);
//       return -1;
//    }

//    return i2c_bus_map[somax_mainboard_id()][somax_i2c_busid];
// }


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


