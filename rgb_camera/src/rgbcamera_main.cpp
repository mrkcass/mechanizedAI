//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: rgbcamera_main.cpp
// DESCRIPTION: Somax rgb camera diagnostics.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "somax.h"
#include "libhardware.h"
#include "mraa.h"
#include "mraa/uart.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define SOMAX_NAME "SOMAX-CAMRGB"
#define SOMAX_CLASS "SOMAX_HARDWARE-VISION-RGB"
#define SOMAX_VERSION "1.0.0"
#define SOMAX_RESOURCES "SOMAX-UART"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static mraa_uart_context camrgb_cam_open();
static void camrgb_cam_close(mraa_uart_context uart);
static void camrgb_display_help();
static int camrgb_info(mraa_uart_context uart);
static int camrgb_configure_uart(mraa_uart_context uart);

static int camrgb_send_command(mraa_uart_context uart, smx_byte * cmd, int cmd_len, smx_byte * reply_buff, int reply_len, bool print_reply);
static bool camrgb_verify_reply(smx_byte * reply_buff, smx_byte * rpl_expected, int expected_len);
static void camrgb_print_reply(const char * banner, smx_byte * reply, int first_letter, int last_letter);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   if (argc > 1 && (somax_commandline_has_option(argc, argv, "help") ||
                    somax_commandline_has_option(argc, argv, "--help") ||
                    somax_commandline_has_option(argc, argv, "-h")))
   {
      camrgb_display_help();
      return 0;
   }

   int return_code = 0;

   printf("\n\n");
   somax_data_init(SOMAX_NAME, SOMAX_CLASS, SOMAX_VERSION, SOMAX_RESOURCES);

   libhardware_init();

   if (!return_code && somax_commandline_has_option(argc, argv, "info"))
   {
      mraa_uart_context uart = camrgb_cam_open();
      if (uart == NULL)
         return_code = 1;
      if (!return_code)
         return_code = camrgb_configure_uart(uart);
      if (!return_code)
         return_code = camrgb_info(uart);
      if (!return_code)
         camrgb_cam_close(uart);
   }
   else if (!return_code && somax_commandline_has_option(argc, argv, "test-rgb-160-120"))
   {
      printf("\n\nFunction not yet implemented\n");
   }
   else
   {
      camrgb_display_help();

      printf("\n\nError: Unknown or missing argument\n\n");
   }

   libhardware_deinit();
   
   if (return_code >= 0)
      return 0;
   else
      return return_code;
}

void camrgb_display_help()
{
   printf("\n");
   printf("  Somax RGB imaging driver control and diagnostics\n");
   printf("  Version: %s\n", SOMAX_VERSION);
   printf("\n");
   printf("  commands:\n");
   printf("    info\n");
   printf("       Read and print RGB camera device information\n");
   printf("    test-rgb-160-120\n");
   printf("       Open camera and aquire a RGB 160 x 120 iamge\n");
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


// static int camrgb_open_cam()
// {
//     mraa_uart_context uart;
//     mraa_result_t status = MRAA_SUCCESS;


//     int baudrate = 115200, stopbits = 1, databits = 8;
//     mraa_uart_parity_t parity = MRAA_UART_PARITY_NONE;
//     unsigned int ctsrts = 0, xonxoff = 0;

//     /* initialize uart tty */
//     uart = mraa_uart_init(0);

//     if (uart == NULL)
//     {
//         fprintf(stderr, "Failed to initialize uart\n");
//         return -1;
//     }

//     status = mraa_uart_set_flowcontrol(uart, ctsrts, xonxoff);
//     if (status != MRAA_SUCCESS)
//     {
//         fprintf(stderr, "Failed to setup flow control\n");
//         return -1;
//     }

//     status = mraa_uart_set_baudrate(uart, baudrate);
//     if (status != MRAA_SUCCESS)
//     {
//         fprintf(stderr, "Failed to setup baud rate\n");
//         return -1;
//     }

//     status = mraa_uart_set_mode(uart, databits, parity, stopbits);
//     if (status != MRAA_SUCCESS)
//     {
//         fprintf(stderr, "Failed to setup mode\n");
//         return -1;
//     }

//    //printf ("** sending reset\n");
//    //camrgb_send_command(uart, cmd_reset, sizeof(cmd_reset), reply, 4);
//    //somax_sleep(U_SECOND*3);

//    // while (1)
//    // {
//    //    printf("******start capture******\n");
//    //    camrgb_send_command(uart, cmd_start, sizeof(cmd_start), reply, 5);
//    //    somax_sleep(U_MILLISECOND*500);

//    //    printf("read length\n");
//    //    camrgb_send_command(uart, cmd_length, sizeof(cmd_length), reply, 5);

//    //    printf("read data\n");
//    //    camrgb_send_command(uart, cmd_read, sizeof(cmd_read), reply, 5);

//    //    printf("stop capture\n");
//    //    camrgb_send_command(uart, cmd_stop, sizeof(cmd_stop), reply, 5);

//    //    somax_sleep(U_SECOND*1);
//    // }

//     /* stop uart */
//     mraa_uart_stop(uart);

//     mraa_deinit();

//     return 0;
// }

static mraa_uart_context camrgb_cam_open()
{
   mraa_uart_context uart = NULL;

   printf ("Opening CAMERA UART ====================================\n");
   /* initialize uart tty */
   uart = mraa_uart_init(0);

   if (uart == NULL)
   {
      fprintf(stderr, "  UART CONNECT: Failed to initialize uart\n");
   }
   else
   {
      const char * device = mraa_uart_get_dev_path(uart);
      fprintf(stdout, " UART CONNECT: opened UART %s\n", device);
   }

   return uart;
}

static int camrgb_configure_uart(mraa_uart_context uart)
{
   mraa_result_t status = MRAA_SUCCESS;

   printf ("\nConfiguring CAMERA UART ====================================\n");

   int baudrate = 115200, stopbits = 1, databits = 8;
   mraa_uart_parity_t parity = MRAA_UART_PARITY_NONE;
   unsigned int ctsrts = 0, xonxoff = 0;

   status = mraa_uart_set_flowcontrol(uart, ctsrts, xonxoff);
   if (status != MRAA_SUCCESS)
   {
      fprintf(stderr, "Failed to setup flow control\n");
      return -1;
   }
   else
   {
      fprintf(stdout, "  CONFIGURATION: software flow control off\n");
      fprintf(stdout, "  CONFIGURATION: hardware flow control off\n");
   }

   status = mraa_uart_set_baudrate(uart, baudrate);
   if (status != MRAA_SUCCESS)
   {
      fprintf(stderr, "Failed to setup baud rate\n");
      return -1;
   }
   else
   {
      fprintf(stdout, "  CONFIGURATION: baud rate %d\n", baudrate);
   }
   

   status = mraa_uart_set_mode(uart, databits, parity, stopbits);
   if (status != MRAA_SUCCESS)
   {
      fprintf(stderr, "Failed to setup mode\n");
      return -1;
   }
   else
      fprintf(stdout, "  CONFIGURATION: data format N-8-1\n");

   printf("UART: configuration complete.\n");

   return 0;
}

static void camrgb_cam_close(mraa_uart_context uart)
{
   printf ("\nClosing CAMERA UART ====================================\n");
   mraa_uart_stop(uart);
}

static int camrgb_info(mraa_uart_context uart)
{
   printf ("\nGetting CAMERA info ====================================\n");

   smx_byte cmd_reset[]          = {0x56, 0x00, 0x26, 0x00};
   smx_byte rpl_reset[]          = {0x76, 0x00, 0x26, 0x00, 0x00};
   
   smx_byte cmd_get_version[]    = {0x56, 0x00, 0x11, 0x00};
   smx_byte rpl_get_version[]    = {0x76, 0x00, 0x11, 0x00, 0x0B};
   //smx_byte cmd_baudrate_9600[] =  {0x56, 0x00, 0x31, 0x06, 0x02, 0x00, 0x08, 0xAE, 0xC8};
   //smx_byte cmd_baudrate_19200[] = {0x56, 0x00, 0x31, 0x06, 0x02, 0x00, 0x08, 0x56, 0xE4};
   //smx_byte cmd_tv_off[]         = {0x56, 0x00, 0x44, 0x01, 0x00};

   smx_byte cmd_resolution[]     = {0x56, 0x00, 0x31, 0x05, 0x04, 0x01, 0x00, 0x19, 0x22};
   smx_byte rpl_resolution[]     = {0x76, 0x00, 0x31, 0x00, 0x00};

   smx_byte cmd_compression[]    = {0x56, 0x00, 0x31, 0x05, 0x01, 0x01, 0x12, 0x04, 0x00};
   smx_byte rpl_compression[]    = {0x76, 0x00, 0x31, 0x00, 0x00};

   smx_byte cmd_mirror_status[]  = {0x56, 0x00, 0x3B, 0x00};
   smx_byte rpl_mirror_status[]  = {0x76, 0x00, 0x3B, 0x00, 0x02};

   smx_byte cmd_color_status[]  = {0x56, 0x00, 0x3D, 0x00};
   smx_byte rpl_color_status[]  = {0x76, 0x00, 0x3D, 0x00, 0x03};

   smx_byte cmd_power_save_enabled_status[]  = {0x56, 0x00, 0x3F, 0x01, 0x00};
   smx_byte rpl_power_save_enabled_status[]  = {0x76, 0x00, 0x3F, 0x00, 0x02};

   smx_byte cmd_power_save_mode_status[]  = {0x56, 0x00, 0x3F, 0x01, 0x01};
   smx_byte rpl_power_save_mode_status[]  = {0x76, 0x00, 0x3F, 0x00, 0x03};

   //smx_byte cmd_start[]          = {0x56, 0x00, 0x36, 0x01, 0x00};
   //smx_byte cmd_stop[]           = {0x56, 0x00, 0x36, 0x01, 0x03};
   //smx_byte cmd_length[]         = {0x56, 0x00, 0x34, 0x01, 0x00};
   //smx_byte cmd_read[]           = {0x56, 0x00, 0x32, 0x0C, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x0A};


   //smx_byte cmd_null[]           = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};



   smx_byte reply[256];
   int reply_len;

   printf("\n");
   printf ("******** Sending reset command ****************\n");
   reply_len = camrgb_send_command(uart, cmd_reset, sizeof(cmd_reset), reply, 59, false);
   camrgb_verify_reply(reply, rpl_reset, sizeof(rpl_reset));
   camrgb_print_reply("reset reply", reply, sizeof(rpl_reset)+1, reply_len-1);
   printf("\n");

   printf ("******** Sending resolution command ***********\n");
   camrgb_send_command(uart, cmd_resolution, sizeof(cmd_resolution), reply, 5, false);
   camrgb_verify_reply(reply, rpl_resolution, sizeof(rpl_resolution));
   printf("\n");

   printf ("******** Sending compression command **********\n");
   camrgb_send_command(uart, cmd_compression, sizeof(cmd_compression), reply, 5, false);
   camrgb_verify_reply(reply, rpl_compression, sizeof(rpl_compression));
   printf("\n");

   printf ("******** Sending version command **************\n");
   reply_len = camrgb_send_command(uart, cmd_get_version, sizeof(cmd_get_version), reply, 16, false);
   camrgb_verify_reply(reply, rpl_get_version, sizeof(rpl_get_version));
   camrgb_print_reply("version reply", reply, sizeof(rpl_get_version), reply_len);
   printf("\n");

   printf ("******** Sending mirror status command **************\n");
   reply_len = camrgb_send_command(uart, cmd_mirror_status, sizeof(cmd_mirror_status), reply, 7, false);
   camrgb_verify_reply(reply, rpl_mirror_status, sizeof(rpl_mirror_status));
   if (reply[reply_len-1] == 0x01)
      reply_len = sprintf((char*)reply, "%s controlled mirror mode enabled", reply[reply_len-2] ? "UART" : "GPIO");
   else
      reply_len = sprintf((char*)reply, "%s controlled mirror mode disabled", reply[reply_len-2] ? "UART" : "GPIO");
   camrgb_print_reply("mirror status reply", reply, 0, reply_len);
   printf("\n");

   printf ("******** Sending color status command **************\n");
   reply_len = camrgb_send_command(uart, cmd_color_status, sizeof(cmd_color_status), reply, 7, false);
   camrgb_verify_reply(reply, rpl_color_status, sizeof(rpl_color_status));
   if (reply[reply_len-2] == 0x00)
      //might be decoding the color/black&white backwards
      reply_len = sprintf((char*)reply, "%s controlled auto select %s", reply[reply_len-3] ? "UART" : "GPIO", reply[reply_len-1] ? "color" : "black&white");
   else if (reply[reply_len-2] == 0x01)
      reply_len = sprintf((char*)reply, "%s controlled manual select color", reply[reply_len-3] ? "UART" : "GPIO");
   else
      reply_len = sprintf((char*)reply, "%s controlled manual select black&white", reply[reply_len-3] ? "UART" : "GPIO");
   camrgb_print_reply("color status reply", reply, 0, reply_len);
   printf("\n");

   printf ("******** Sending power save enabled status command **************\n");
   reply_len = camrgb_send_command(uart, cmd_power_save_enabled_status, sizeof(cmd_power_save_enabled_status), reply, 7, false);
   camrgb_verify_reply(reply, rpl_power_save_enabled_status, sizeof(rpl_power_save_enabled_status));
   if (reply[reply_len-1] == 0x00)
      reply_len = sprintf((char*)reply, "%s controlled power-save off", reply[reply_len-2] ? "UART" : "GPIO");
   else if (reply[reply_len-1] == 0x01)
      reply_len = sprintf((char*)reply, "%s controlled power-save on", reply[reply_len-2] ? "UART" : "GPIO");
   camrgb_print_reply("power-save enabled reply", reply, 0, reply_len);
   printf("\n");

   printf ("******** Sending power save mode status command **************\n");
   reply_len = camrgb_send_command(uart, cmd_power_save_mode_status, sizeof(cmd_power_save_mode_status), reply, 7, false);
   camrgb_verify_reply(reply, rpl_power_save_mode_status, sizeof(rpl_power_save_mode_status));
   if (reply[reply_len-3] & 0x02)
      reply_len = sprintf((char*)reply, "motion activated %s shutdown in 0x%X 0x%X", reply[reply_len-3] & 0x01 ? "frame buffer" : "jpeg compressor", reply[reply_len-2], reply[reply_len-1]);
   else
      reply_len = sprintf((char*)reply, "timer activated %s shutdown in 0x%X 0x%X", reply[reply_len-3] & 0x01 ? "frame buffer" : "jpeg compressor", reply[reply_len-2], reply[reply_len-1]);
   camrgb_print_reply("power-save mode reply", reply, 0, reply_len);
   printf("\n");

   return 0;
}

static int camrgb_send_command(mraa_uart_context uart, smx_byte * cmd, int cmd_len, smx_byte * reply_buff, int reply_len, bool print_reply)
{
   int bytes_written, bytes_read;
   mraa_uart_flush(uart);
   printf ("  UART: flushed, writing %d bytes\n", cmd_len);
   bytes_written = mraa_uart_write(uart, (char*)cmd, cmd_len);
   smx_byte overflow_buff[256];
   int overflow_read = 0;
   bytes_read = 0;

   if (bytes_written)
   {
      printf("  UART: wrote %d bytes. wating for reply...\n", bytes_written);
      
      while (bytes_read < reply_len || overflow_read < 256)
      {
         if (!mraa_uart_data_available(uart, 100))
         {
            if (bytes_read == reply_len)
               break;
            continue;
         }
         if (bytes_read == reply_len)
            overflow_read += mraa_uart_read(uart, (char*)&overflow_buff[overflow_read], 1);
         else
            bytes_read += mraa_uart_read(uart, (char*)&reply_buff[bytes_read], 1);
      }
      if (print_reply)
      {
         if (bytes_read)
         {
            printf("  UART: received reply. ");
            for (int i=0; i < bytes_read; i++)
               printf ("0x%X ", reply_buff[i]);
            printf("\n");
         }
         if (overflow_read)
         {
            printf("  UART: received overflow. ");
            for (int i=0; i < overflow_read; i++)
               printf ("0x%X ", overflow_buff[i]);
            printf("\n");
         }
      }
   }
   else
        printf("write msg failed\n");

   return bytes_read;
}

static bool camrgb_verify_reply(smx_byte * reply_buff, smx_byte * rpl_expected, int expected_len)
{
   bool passed = false;

   fprintf(stdout, "reply status ----------------------\n");
   if (memcmp(reply_buff, rpl_expected, expected_len) == 0)
   {
      passed = true;
      fprintf(stdout, "PASSED: ");
   }
   else
   {
      fprintf(stdout, "FAILED: ");
   }
   
   for (int i=0; i < expected_len; i++)
         fprintf (stdout, "0x%X ", reply_buff[i]);
   fprintf(stdout, "\n");

   return passed;
}

static void camrgb_print_reply(const char * banner, smx_byte * reply, int first_letter, int last_letter)
{
   fprintf(stdout, "reply string ----------------------\n");

   char * text_reply = (char*)&reply[first_letter];
   text_reply[last_letter-first_letter] = '\0';
   fprintf(stdout, "%s\n", text_reply);
}

