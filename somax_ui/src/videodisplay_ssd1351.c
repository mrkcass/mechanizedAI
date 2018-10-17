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

#include "somax.h"
#include "spi_interface.h"
#include "videodisplay_ssd1351.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//SSD1351 command and configuration constants / data
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define SSD1351_CMD_SETCOLUMN       0x15
#define SSD1351_CMD_SETROW          0x75
#define SSD1351_CMD_WRITERAM        0x5C
#define SSD1351_CMD_READRAM         0x5D
#define SSD1351_CMD_SETREMAP        0xA0
#define SSD1351_CMD_STARTLINE       0xA1
#define SSD1351_CMD_DISPLAYOFFSET   0xA2
#define SSD1351_CMD_DISPLAYALLOFF   0xA4
#define SSD1351_CMD_DISPLAYALLON    0xA5
#define SSD1351_CMD_NORMALDISPLAY   0xA6
#define SSD1351_CMD_INVERTDISPLAY   0xA7
#define SSD1351_CMD_FUNCTIONSELECT  0xAB
#define SSD1351_CMD_DISPLAYOFF      0xAE
#define SSD1351_CMD_DISPLAYON       0xAF
#define SSD1351_CMD_PRECHARGE       0xB1
#define SSD1351_CMD_DISPLAYENHANCE  0xB2
#define SSD1351_CMD_CLOCKDIV        0xB3
#define SSD1351_CMD_SETVSL          0xB4
#define SSD1351_CMD_SETGPIO         0xB5
#define SSD1351_CMD_PRECHARGE2      0xB6
#define SSD1351_CMD_SETGRAY         0xB8
#define SSD1351_CMD_USELUT          0xB9
#define SSD1351_CMD_PRECHARGELEVEL  0xBB
#define SSD1351_CMD_VCOMH           0xBE
#define SSD1351_CMD_CONTRASTABC     0xC1
#define SSD1351_CMD_CONTRASTMASTER  0xC7
#define SSD1351_CMD_MUXRATIO        0xCA
#define SSD1351_CMD_COMMANDLOCK     0xFD
#define SSD1351_CMD_HORIZSCROLL     0x96
#define SSD1351_CMD_STOPSCROLL      0x9E
#define SSD1351_CMD_STARTSCROLL     0x9F

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA STRUCTURES
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct SSD1351_DEVICE
{
   spi_busid spi_bus_id;
   int spi_chip_select;
   int spi_mode;
   spi_context spi_bus;
};
typedef SSD1351_DEVICE* ssd1351_device;
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool ssd1351_init(ssd1351_device device);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static SSD1351_DEVICE devices[SSD1351_NUM_DEVICEID+1] =
{
   {0,0,0,0},
   {
      SPI_BUSID_0,
      SPI_CHIPSELECT_0,
      SPI_MODE_0,
      25000,
      0,
   },
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool ssd1351_ini_open(viddisp_deviceid device_id)
{
   if (devices[device_id].spi_bus != 0)
   {
      somax_log_add(SOMAX_LOG_ERR, "SSD1351. open. can't open device(%d) already in use.", device_id);
      return false;
   }

   SSD1351_DEVICE* device = &devices[device_id];
   device->spi_bus = spi_ini_open(device->spi_bus_id, device->spi_chip_select);

   spi_cfg_reset(device->spi_bus, 500 * U_MILLISECOND);

   ssd1351_init(device);

   return true;
}

void ssd1351_ini_close(viddisp_deviceid id)
{

}

void ssd1351_opr_display_buffer(viddisp_deviceid id, viddisp_frame_buffer buffer)
{

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool ssd1351_init(ssd1351_device device)
{
   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_COMMANDLOCK);
   spi_dat_write_byte(device->spi_bus, 0x12);

   //todo: should ther be 2 command locks
   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_COMMANDLOCK);
   spi_dat_write_byte(device->spi_bus, 0xB1);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_DISPLAYOFF);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_SETCOLUMN);
   spi_dat_write_byte(device->spi_bus, 0x00);
   spi_dat_write_byte(device->spi_bus, 0x7F);
}