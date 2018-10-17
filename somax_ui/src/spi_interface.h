#ifndef __spi_interface_h__
#define __spi_interface_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: spi_interface.h
// DESCRIPTION: Interface for SPI bus communication.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include <stdint.h>

#define SPI_BUSID_0     1
#define SPI_NUM_BUSID   1

#define SPI_CHIPSELECT_0   0
#define SPI_CHIPSELECT_1   1

#define SPI_MODE_0   0
#define SPI_MODE_1   1
#define SPI_MODE_2   2
#define SPI_MODE_3   3

#define SPI_FREQUENCY_MIN_KHZ       400
#define SPI_FREQUENCY_MAX_KHZ       125000   //125 MHZ
#define SPI_FREQUENCY_DEFAULT_KHZ   25000    //25 MHZ

#define SPI_MAX_CONTEXTS 32
struct SPI_CONTEXT;
typedef SPI_CONTEXT* spi_context;

typedef int spi_busid;

spi_context spi_ini_open(spi_busid bus_id, int chip_select);
void spi_ini_close(spi_context spi);

int spi_cfg_frequency(spi_context spi, int freq_khz);
void spi_cfg_mode(spi_context spi, int spi_mode);
void spi_cfg_reset(spi_context spi, smx_ui32 reset_toggle_delay);

void spi_cmd_write_byte(spi_context spi, uint8_t byte);
void spi_cmd_write_word(spi_context spi, uint16_t word);
void spi_cmd_write_bytes(spi_context spi, uint8_t *bytes, int num_bytes);
void spi_cmd_write_words(spi_context spi, uint16_t *words, int num_words);
void spi_dat_write_byte(spi_context spi, uint8_t byte);
void spi_dat_write_word(spi_context spi, uint16_t word);
void spi_dat_write_bytes(spi_context spi, uint8_t *bytes, int num_bytes);
void spi_dat_write_words(spi_context spi, uint16_t *words, int num_words);



#endif