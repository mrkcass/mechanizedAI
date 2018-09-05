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
#include "adc50.h"

#define SOMAX_NAME "SOMAX-ADC50"
#define SOMAX_CLASS "SOMAX_HARDWARE-ANALOG-SAMPLER"
#define SOMAX_VERSION "1.0.0"
#define SOMAX_RESOURCES "SOMAX-I2C-1"



bool adconverter_display_help(int argc, char *argv[]);
int adconverter_test_adc();
int adconverter_adc_open();
int adconverter_adc_close();


int main(int argc, char *argv[])
{
   somax_data_init(SOMAX_NAME, SOMAX_CLASS, SOMAX_VERSION, SOMAX_RESOURCES);

   if (argc > 1 && !somax_commandline_options_handler(argc, argv))
   {
      if (adconverter_display_help(argc, argv))
         return 0;
   }

   int return_code = 0;

   mraa_init();

   return_code = adconverter_adc_open();

   if (!return_code && somax_commandline_has_option(argc, argv, "test-sampler"))
      adconverter_test_adc();

   adconverter_adc_close();
   mraa_deinit();
   if (return_code > 1)
      return 0;
   else
      return return_code;
}

bool adconverter_display_help(int argc, char *argv[])
{
   int show_help = false;
   if (!strcmp(argv[1], "help") ||
         !strcmp(argv[1], "--help") ||
         !strcmp(argv[1], "-h"))
      show_help = true;

   if (!show_help)
      return false;

   printf("\n");
   printf("  Check bus and dunp contents of config register\n");
   printf("\n");
   printf("Commands\n");
   printf("    test-sampler - Sample anolog to digital convter and report returned values to stdout\n");
   printf("                   until control-c.\n");

   return true;
}

int adconverter_adc_open()
{

   adc50_open(ADC50_INPUT_DEFAULT);

   return 0;
}

int adconverter_adc_close()
{
   adc50_close();

   return 0;
}

int adconverter_test_adc()
{
   printf("Sampling ADC:\n");
   while (1)
   {
      int a0 = adc50_sample_single_end(0);
      int a1 = adc50_sample_single_end(1);
      int a2 = adc50_sample_single_end(2);
      int a3 = adc50_sample_single_end(3);

      printf("   INPUT-A0: %6d  INPUT-A1: %6d  INPUT-A2: %6d  INPUT-A3: %6d\r", a0, a1, a2, a3);
      fflush(stdout);
      usleep(1000000 / 20);
   }
   return 0;
}