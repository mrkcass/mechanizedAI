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
   int spi_frequency;
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
   {0,0,0,0,0},
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

void ssd1351_opr_display_buffer(viddisp_deviceid device_id, viddisp_frame_buffer buffer)
{
   SSD1351_DEVICE *device = &devices[device_id];

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_SETCOLUMN);
   spi_dat_write_byte(device->spi_bus, 0);
   spi_dat_write_byte(device->spi_bus, SSD1351_WIDTH - 1);
   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_SETROW);
   spi_dat_write_byte(device->spi_bus, 0);
   spi_dat_write_byte(device->spi_bus, SSD1351_HEIGHT - 1);
   // fill!
   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_WRITERAM);

   int max_tx_lines = 4;
   for (uint16_t i = 0; i < SSD1351_HEIGHT / max_tx_lines; i++)
      spi_dat_write_bytes(device->spi_bus, &buffer[SSD1351_WIDTH * i * max_tx_lines * 2], SSD1351_WIDTH * max_tx_lines * 2);
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

   //second command lock is intentional
   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_COMMANDLOCK);
   spi_dat_write_byte(device->spi_bus, 0xB1);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_DISPLAYOFF);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_SETCOLUMN);
   spi_dat_write_byte(device->spi_bus, 0x00);
   spi_dat_write_byte(device->spi_bus, 0x7F);



   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_SETROW);
   spi_dat_write_byte(device->spi_bus, 0x00);
   spi_dat_write_byte(device->spi_bus, 0x7F);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_CLOCKDIV);
   spi_cmd_write_byte(device->spi_bus, 0xF1);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_MUXRATIO);
   spi_dat_write_byte(device->spi_bus, 0x7F);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_SETREMAP);
   spi_dat_write_byte(device->spi_bus, 0x74);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_STARTLINE);
   if (SSD1351_HEIGHT == 96)
      spi_dat_write_byte(device->spi_bus, 96);
   else
      spi_dat_write_byte(device->spi_bus, 0);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_DISPLAYOFFSET);
   spi_dat_write_byte(device->spi_bus, 0x0);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_SETGPIO);
   spi_dat_write_byte(device->spi_bus, 0x00);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_FUNCTIONSELECT);
   spi_dat_write_byte(device->spi_bus, 0x01);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_SETVSL );
   spi_dat_write_byte(device->spi_bus, 0xA0);
   spi_dat_write_byte(device->spi_bus, 0xB5);
   spi_dat_write_byte(device->spi_bus, 0x55);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_CONTRASTABC);
   spi_dat_write_byte(device->spi_bus, 0xC8);
   spi_dat_write_byte(device->spi_bus, 0xC8);
   spi_dat_write_byte(device->spi_bus, 0xC8);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_CONTRASTMASTER);
   spi_dat_write_byte(device->spi_bus, 0x0F);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_PRECHARGE);
   spi_cmd_write_byte(device->spi_bus, 0x32);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_DISPLAYENHANCE);
   spi_dat_write_byte(device->spi_bus, 0xA4);
   spi_dat_write_byte(device->spi_bus, 0x00);
   spi_dat_write_byte(device->spi_bus, 0x00);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_PRECHARGE2);
   spi_cmd_write_byte(device->spi_bus, 0x01);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_VCOMH);
   spi_cmd_write_byte(device->spi_bus, 0x05);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_NORMALDISPLAY);

   spi_cmd_write_byte(device->spi_bus, SSD1351_CMD_DISPLAYON);

   return true;
}