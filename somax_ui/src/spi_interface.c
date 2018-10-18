//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: spi_interface.cpp
// DESCRIPTION: Interface for SPI bus communication.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <stdio.h>

#include "mraa.h"
#include "mraa/spi.h"

#include "somax.h"
#include "spi_interface.h"
#include "gpio_interface.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define TEST_REG_ERROR -1

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
extern char *decode_error(mraa_result_t code);

#if SOMAX_MAINBOARDID == SOMAX_MAINBOARDID_EDISON
spi_context edison_ini_init(spi_busid bus_id, int chip_select);
void edison_ini_deinit(spi_context ctx);
int edison_cfg_frequency(spi_context ctx, int freq_khz);
void edison_cfg_mode(spi_context ctx, int mode);
void edison_cfg_reset(spi_context ctx, smx_ui32 reset_toggle_delay);
void edison_write_cmd_byte(spi_context ctx, uint8_t byte);
void edison_write_cmd_word(spi_context ctx, uint16_t word);
void edison_write_cmd_bytes(spi_context ctx, uint8_t *bytes, int num_bytes);
void edison_write_cmd_words(spi_context ctx, uint16_t *words, int num_words);
void edison_write_dat_byte(spi_context ctx, uint8_t byte);
void edison_write_dat_word(spi_context ctx, uint16_t word);
void edison_write_dat_bytes(spi_context ctx, uint8_t *bytes, int num_bytes);
void edison_write_dat_words(spi_context ctx, uint16_t *words, int num_words);
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA STRUCTURES
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct SPI_CONTEXT
{
    spi_busid bus_id;
    int chipselect;
    int context_slot;
};

typedef spi_context (*bus_init)(spi_busid bus_id, int chip_select);
typedef void (*bus_deinit)(spi_context ctx);
typedef int  (*bus_set_frequency)(spi_context ctx, int freq_khz);
typedef void (*bus_set_mode)(spi_context ctx, int mode);
typedef void (*bus_reset)(spi_context ctx, smx_ui32 reset_toggle_delay);
typedef void (*bus_write_cmd_byte)(spi_context ctx, uint8_t byte);
typedef void (*bus_write_cmd_word)(spi_context ctx, uint16_t word);
typedef void (*bus_write_cmd_bytes)(spi_context ctx, uint8_t* bytes, int num_bytes);
typedef void (*bus_write_cmd_words)(spi_context ctx, uint16_t* words, int num_words);
typedef void (*bus_write_dat_byte)(spi_context ctx, uint8_t byte);
typedef void (*bus_write_dat_word)(spi_context ctx, uint16_t word);
typedef void (*bus_write_dat_bytes)(spi_context ctx, uint8_t *bytes, int num_bytes);
typedef void (*bus_write_dat_words)(spi_context ctx, uint16_t *words, int num_words);

struct SPI_BUS_FUNCS
{
   bus_init             ini_init;
   bus_deinit           ini_deinit;
   bus_set_frequency    cfg_frequency;
   bus_set_mode         cfg_mode;
   bus_reset            cfg_reset;
   bus_write_cmd_byte   write_cmd_byte;
   bus_write_cmd_word   write_cmd_word;
   bus_write_cmd_bytes  write_cmd_bytes;
   bus_write_cmd_words  write_cmd_words;
   bus_write_dat_byte   write_dat_byte;
   bus_write_dat_word   write_dat_word;
   bus_write_dat_bytes  write_dat_bytes;
   bus_write_dat_words  write_dat_words;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static struct SPI_BUS_FUNCS spi_bus_ops[SPI_NUM_BUSID+1] =
{
   {0,0,0,0,0,0,0,0,0,0,0,0,0},
   #if SOMAX_MAINBOARDID == SOMAX_MAINBOARDID_EDISON
   {
      edison_ini_init,
      edison_ini_deinit,
      edison_cfg_frequency,
      edison_cfg_mode,
      edison_cfg_reset,
      edison_write_cmd_byte,
      edison_write_cmd_word,
      edison_write_cmd_bytes,
      edison_write_cmd_words,
      edison_write_dat_byte,
      edison_write_dat_word,
      edison_write_dat_bytes,
      edison_write_dat_words,
   },
   #elif SOMAX_MAINBOARDID == SOMAX_MAINBOARDID_HIKEY960
   {0,0,0,0,0},
   {0,0,0,0,0},
   #endif
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

spi_context spi_ini_open(spi_busid bus_id, int chip_select)
{
   return spi_bus_ops[bus_id].ini_init(bus_id, chip_select);
}

void spi_ini_close(spi_context spi)
{
   if (spi == NULL)
      return;

   if (spi->bus_id)
   {
      spi_bus_ops[spi->bus_id].ini_deinit(spi);
   }
}

int spi_cfg_frequency(spi_context spi, int freq_khz)
{
   return spi_bus_ops[spi->bus_id].cfg_frequency(spi, freq_khz);
}

void spi_cfg_reset(spi_context spi, smx_ui32 reset_toggle_delay)
{
   spi_bus_ops[spi->bus_id].cfg_reset(spi, reset_toggle_delay);
}

void spi_cfg_mode(spi_context spi, int mode)
{
   spi_bus_ops[spi->bus_id].cfg_mode(spi, mode);
}

void spi_cmd_write_byte(spi_context spi, uint8_t byte)
{
   spi_bus_ops[spi->bus_id].write_cmd_byte(spi, byte);
}

void spi_cmd_write_word(spi_context spi, uint16_t word)
{
   spi_bus_ops[spi->bus_id].write_cmd_word(spi, word);
}

void spi_cmd_write_bytes(spi_context spi, uint8_t *bytes, int num_bytes)
{
   spi_bus_ops[spi->bus_id].write_cmd_bytes(spi, bytes, num_bytes);
}

void spi_cmd_write_words(spi_context spi, uint16_t *words, int num_words)
{
   spi_bus_ops[spi->bus_id].write_cmd_words(spi, words, num_words);
}

void spi_dat_write_byte(spi_context spi, uint8_t byte)
{
   spi_bus_ops[spi->bus_id].write_dat_byte(spi, byte);
}

void spi_dat_write_word(spi_context spi, uint16_t word)
{
   spi_bus_ops[spi->bus_id].write_dat_word(spi, word);
}

void spi_dat_write_bytes(spi_context spi, uint8_t *bytes, int num_bytes)
{
   spi_bus_ops[spi->bus_id].write_dat_bytes(spi, bytes, num_bytes);
}

void spi_dat_write_words(spi_context spi, uint16_t *words, int num_words)
{
   spi_bus_ops[spi->bus_id].write_dat_words(spi, words, num_words);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#if SOMAX_MAINBOARDID == SOMAX_MAINBOARDID_EDISON
#define SPI_EDISON_NUM_BUS 1
#define GPIO_SPI0_RESET    0
#define GPIO_SPI0_DC       1
#define GPIO_STATE_CMD     GPIO_STATE_LO
#define GPIO_STATE_DAT     GPIO_STATE_HI
struct EDISON_SPI_BUS
{
   struct SPI_CONTEXT contexts[SPI_MAX_CONTEXTS];
   int context_count;
   mraa_spi_context spi;
   gpio_context dc; //data / command
   gpio_context reset;
};
typedef struct EDISON_SPI_BUS *edison_spi_bus;
static struct EDISON_SPI_BUS edison_bus[SPI_EDISON_NUM_BUS+1];
static int edison_spi_map[SPI_EDISON_NUM_BUS+1] =
{
   0,
   1, //edison spi 1
};

spi_context edison_ini_init(spi_busid bus_id, int chip_select)
{
   mraa_result_t result;
   edison_spi_bus bus = &edison_bus[bus_id];

   if (!bus->spi)
   {
      printf("\nInitalizing SPI..\n");
      mraa_spi_context spi = mraa_spi_init(edison_spi_map[bus_id]);
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
      bus->spi = spi;
      bus->reset = gpio_ini_open(GPIO_CONTROLLERID_MAINBOARD, GPIO_SPI0_RESET, GPIO_DIRECTION_OUT);
      bus->dc = gpio_ini_open(GPIO_CONTROLLERID_MAINBOARD, GPIO_SPI0_DC, GPIO_DIRECTION_OUT);
   }

   spi_context new_ctx = &bus->contexts[bus->context_count];
   new_ctx->bus_id = bus_id;
   new_ctx->chipselect = chip_select;
   new_ctx->context_slot = bus->context_count;

   bus->context_count++;

   return new_ctx;
}

void edison_ini_deinit(spi_context ctx)
{
   //todo: implement edison_deinit
}

int edison_cfg_frequency(spi_context ctx, int freq_khz)
{
   if (freq_khz < SPI_FREQUENCY_MIN_KHZ || freq_khz > SPI_FREQUENCY_MAX_KHZ)
   {
      somax_log_add(SOMAX_LOG_ERR, "SPI: set frequency. requested out of range %d khz", freq_khz);
   }

   mraa_result_t result = mraa_spi_frequency(edison_bus[ctx->bus_id].spi, freq_khz * 1000);
   if (result != MRAA_SUCCESS)
   {
      somax_log_add(SOMAX_LOG_WARN, "SPI. set_frequency failed at %d khz", freq_khz);
      return 0;
   }
   somax_log_add(SOMAX_LOG_INFO, "SPI. bus %d. set_frequency to %d khz", ctx->bus_id - SPI_BUSID_0, freq_khz);
   return freq_khz;
}

void edison_cfg_mode(spi_context ctx, int mode)
{
   edison_spi_bus bus = &edison_bus[ctx->bus_id];

   mraa_result_t result = mraa_spi_mode(bus->spi, MRAA_SPI_MODE0);
   if (result == MRAA_SUCCESS)
      printf("  SPI: mode = 0\n");
   else
      printf("  SPI ERROR: Could not set mode = 0. [%d]%s\n", result, decode_error(result));
}

void edison_cfg_reset(spi_context ctx, smx_ui32 reset_toggle_delay)
{
   edison_spi_bus bus = &edison_bus[ctx->bus_id];
   printf("debug3.2.1 %p\n", bus->reset);
   gpio_cfg_state(bus->reset, GPIO_STATE_HI);
   somax_sleep(reset_toggle_delay);
   printf("debug3.2.2\n");
   gpio_cfg_state(bus->reset, GPIO_STATE_LO);
   somax_sleep(reset_toggle_delay);
   printf("debug3.2.3\n");
   gpio_cfg_state(bus->reset, GPIO_STATE_HI);
   somax_sleep(reset_toggle_delay);
}

void edison_write_cmd_byte(spi_context ctx, uint8_t byte)
{
   edison_spi_bus bus = &edison_bus[ctx->bus_id];
   gpio_cfg_state(bus->dc, GPIO_STATE_CMD);
   int status = mraa_spi_write(bus->spi, byte);
   if (status < 0)
      somax_log_add(SOMAX_LOG_ERR, "SPI(%d). write command byte failed [%x]\n", ctx->bus_id, byte);
   gpio_cfg_state(bus->dc, GPIO_STATE_DAT);
}

void edison_write_cmd_word(spi_context ctx, uint16_t word)
{
   edison_spi_bus bus = &edison_bus[ctx->bus_id];
   gpio_cfg_state(bus->dc, GPIO_STATE_CMD);
   int status = mraa_spi_write_word(bus->spi, word);
   if (status < 0)
      somax_log_add(SOMAX_LOG_ERR, "SPI(%d). write command word failed [%x]\n", ctx->bus_id, word);
   gpio_cfg_state(bus->dc, GPIO_STATE_DAT);
}

void edison_write_cmd_bytes(spi_context ctx, uint8_t *bytes, int num_bytes)
{
   edison_spi_bus bus = &edison_bus[ctx->bus_id];
   gpio_cfg_state(bus->dc, GPIO_STATE_CMD);
   mraa_result_t status = mraa_spi_transfer_buf(bus->spi, bytes, 0, num_bytes);
   if (status != MRAA_SUCCESS)
      somax_log_add(SOMAX_LOG_ERR, "SPI(%d). write command bytes failed\n", ctx->bus_id);
   gpio_cfg_state(bus->dc, GPIO_STATE_DAT);
}

void edison_write_cmd_words(spi_context ctx, uint16_t *words, int num_words)
{
   edison_spi_bus bus = &edison_bus[ctx->bus_id];
   gpio_cfg_state(bus->dc, GPIO_STATE_CMD);
   mraa_result_t status = mraa_spi_transfer_buf_word(bus->spi, words, 0, num_words);
   if (status != MRAA_SUCCESS)
      somax_log_add(SOMAX_LOG_ERR, "SPI(%d). write command words failed\n", ctx->bus_id);
   gpio_cfg_state(bus->dc, GPIO_STATE_DAT);
}

void edison_write_dat_byte(spi_context ctx, uint8_t byte)
{
   edison_spi_bus bus = &edison_bus[ctx->bus_id];
   gpio_cfg_state(bus->dc, GPIO_STATE_DAT);
   int status = mraa_spi_write(bus->spi, byte);
   if (status < 0)
      somax_log_add(SOMAX_LOG_ERR, "SPI(%d). write command byte failed [%x]\n", ctx->bus_id, byte);
   gpio_cfg_state(bus->dc, GPIO_STATE_CMD);
}

void edison_write_dat_word(spi_context ctx, uint16_t word)
{
   edison_spi_bus bus = &edison_bus[ctx->bus_id];
   gpio_cfg_state(bus->dc, GPIO_STATE_DAT);
   int status = mraa_spi_write_word(bus->spi, word);
   if (status < 0)
      somax_log_add(SOMAX_LOG_ERR, "SPI(%d). write command word failed [%x]\n", ctx->bus_id, word);
   gpio_cfg_state(bus->dc, GPIO_STATE_CMD);
}

void edison_write_dat_bytes(spi_context ctx, uint8_t *bytes, int num_bytes)
{
   edison_spi_bus bus = &edison_bus[ctx->bus_id];
   gpio_cfg_state(bus->dc, GPIO_STATE_DAT);
   mraa_result_t status = mraa_spi_transfer_buf(bus->spi, bytes, 0, num_bytes);
   if (status != MRAA_SUCCESS)
      somax_log_add(SOMAX_LOG_ERR, "SPI(%d). write command bytes failed\n", ctx->bus_id);
   gpio_cfg_state(bus->dc, GPIO_STATE_CMD);
}

void edison_write_dat_words(spi_context ctx, uint16_t *words, int num_words)
{
   edison_spi_bus bus = &edison_bus[ctx->bus_id];
   gpio_cfg_state(bus->dc, GPIO_STATE_DAT);
   mraa_result_t status = mraa_spi_transfer_buf_word(bus->spi, words, 0, num_words);
   if (status != MRAA_SUCCESS)
      somax_log_add(SOMAX_LOG_ERR, "SPI(%d). write command words failed\n", ctx->bus_id);
   gpio_cfg_state(bus->dc, GPIO_STATE_CMD);
}

#endif