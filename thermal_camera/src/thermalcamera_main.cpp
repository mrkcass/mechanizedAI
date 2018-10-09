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
#include "thermalcamera.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define SOMAX_NAME "SOMAX-CAMTHERM"
#define SOMAX_CLASS "SOMAX_HARDWARE-VISION-THERMAL"
#define SOMAX_VERSION "1.0.0"
#define SOMAX_RESOURCES "SOMAX-I2C6"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static thermcam_context thermcamctl_camt;

static int thermcamctl_open_camt();
static void thermcamctl_close_camt();
static void thermcamctl_display_help();
static void thermcamctl_info();
static void thermcamctl_test();

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
      thermcamctl_display_help();
      return 1;
   }

   int return_code = 0;

   printf("\n\n");
   somax_data_init(SOMAX_NAME, SOMAX_CLASS, SOMAX_VERSION, SOMAX_RESOURCES);

   libhardware_init();

   return_code = thermcamctl_open_camt();

   if (!return_code && somax_commandline_has_option(argc, argv, "info"))
   {
      thermcamctl_info();
   }
   else if (!return_code && somax_commandline_has_option(argc, argv, "test"))
   {
      thermcamctl_info();
      thermcamctl_test();
   }

   thermcamctl_close_camt();
   libhardware_deinit();
   if (return_code >= 0)
      return 0;
   else
      return return_code;
}

void thermcamctl_display_help()
{
   printf("\n");
   printf("  Somax thermal imaging driver control and diagnostics\n");
   printf("  Version: %s\n", SOMAX_VERSION);
   printf("\n");
   printf("  commands:\n");
   printf("    info\n");
   printf("       Read and print thermal camera device information\n");
   printf("    test-console\n");
   printf("       Intialize thermal camera and output raw frame data to the console.\n");
   printf("       until control-c.\n");
   printf("    run-normal\n");
   printf("       Initialize thermal camera and \n");
   printf("       until control-c.\n");
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static int thermcamctl_open_camt()
{
   thermcamctl_camt = thermcam_open(THERMCAMID_GIMBAL);

   if (thermcamctl_camt == THERMCAM_CONTEXT_NULL)
   {
      somax_log_add(SOMAX_LOG_ERR, "THERMALCAMERA: could not be started");
      printf("THERMALCAMERA: could not be started\n");
   }

   somax_log_add(SOMAX_LOG_INFO, "THERMALCAMERA: started");

   return 0;
}

static void thermcamctl_close_camt()
{
   thermcam_close(thermcamctl_camt);
}

static void thermcamctl_info()
{
   thermcam_info(thermcamctl_camt);
}

static void thermcamctl_test()
{
   thermcam_diagnostics(thermcamctl_camt, THERMCAM_DIAGNOSTICID_CONSOLE_OUTPUT);
}