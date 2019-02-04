//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: thermalcamera_main.cpp
// DESCRIPTION: Somax thermal imaging service and diagnostic entry point.
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
#include "lidarcamera.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define SOMAX_NAME "SOMAX-CAMLIDAR"
#define SOMAX_CLASS "SOMAX_HARDWARE-VISION-DEPTH"
#define SOMAX_VERSION "1.0.0"
#define SOMAX_RESOURCES "SOMAX-I2C6"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static lidarcam_context lidarcam;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static int lidarcamctl_open_cam();
static void lidarcamctl_close_cam();
static void lidarcamctl_display_help();
static void lidarcamctl_info();

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
      lidarcamctl_display_help();
      return 1;
   }

   int return_code = 0;

   printf("\n\n");
   somax_data_init(SOMAX_NAME, SOMAX_CLASS, SOMAX_VERSION, SOMAX_RESOURCES);

   libhardware_init();

   return_code = lidarcamctl_open_cam();

   if (!return_code && somax_commandline_has_option(argc, argv, "info"))
   {
      lidarcamctl_info();
   }
   else if (!return_code && somax_commandline_has_option(argc, argv, "test-dist-mm"))
   {
      lidarcamctl_info();
      lidarcam_diagnostics(lidarcam, LIDARCAM_DIAGNOSTICID_FRAMEDATA_CONSOLE_OUTPUT);
   }
   else if (!return_code && somax_commandline_has_option(argc, argv, "test-pixeldata-devtemp"))
   {
      lidarcamctl_info();
   }
   else
   {
      lidarcamctl_display_help();

      printf("\n\nError: Unknown or missing argument\n\n");
   }

   lidarcamctl_close_cam();
   libhardware_deinit();
   if (return_code >= 0)
      return 0;
   else
      return return_code;
}

void lidarcamctl_display_help()
{
   printf("\n");
   printf("  Somax lidar imaging driver control and diagnostics\n");
   printf("  Version: %s\n", SOMAX_VERSION);
   printf("\n");
   printf("  commands:\n");
   printf("    info\n");
   printf("       Read and print lidar camera device information\n");
   printf("    test-dist-mm\n");
   printf("       Init lidar camera, read distance, then print distance\n");
   printf("       in millimeters until ctrl-c\n");
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static int lidarcamctl_open_cam()
{
   lidarcam = lidarcam_open(LIDARCAMID_LEFT);

   if (lidarcam == LIDARCAM_CONTEXT_NULL)
   {
      somax_log_add(SOMAX_LOG_ERR, "LIDARCAMERA: could not be started");
      return 1;
   }

   somax_log_add(SOMAX_LOG_INFO, "LIDARCAMERA: started");

   return 0;
}

static void lidarcamctl_close_cam()
{
}

static void lidarcamctl_info()
{
   lidarcam_info(lidarcam);
   printf("\n");
}
