#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "somax.h"

#define SOMAX_PRINTOPTION_CONFIG 0x01

struct SOMAXDATA
{
   char name[256];
   char sclass[256];
   char version[256];
   char resources[256];
};

static struct SOMAXDATA somax_data;

void somax_data_init(const char name[], const char sclass[], const char version[], const char resources[])
{
   strcpy(somax_data.name, name);
   strcpy(somax_data.sclass, sclass);
   strcpy(somax_data.version, version);
   strcpy(somax_data.resources, resources);
}

void somax_print_config(int print_options)
{
   if (print_options & SOMAX_PRINTOPTION_CONFIG)
   {
      printf("\n");
      printf("Somax Configuration Data ------------------------\n");
      printf("   Name     : %s\n", somax_data.name);
      printf("   Class    : %s\n", somax_data.sclass);
      printf("   Version  : %s\n", somax_data.version);
      printf("   Resources: %s\n", somax_data.resources);
   }
}

bool somax_commandline_options_handler(int argc, char * argv[])
{
   if (argc > 1)
   {
      if (!strcmp(argv[1], "show-config"))
      {
         somax_print_config(SOMAX_PRINTOPTION_CONFIG);
         return true;
      }
   }
   return false;
}

bool somax_commandline_has_option(int argc, char *argv[], const char option_name[])
{
   if (argc > 1)
   {
      if (!strcmp(argv[1], option_name))
      {
         return true;
      }
   }
   return false;
}

void somax_log_add(int log_level, const char *fmt, ...)
{
   va_list args;
   va_start(args, fmt);
   vprintf(fmt, args);
   va_end(args);
}