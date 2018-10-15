//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: somax.c
// DESCRIPTION: Somax constants and utility functions.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "somax.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define SOMAX_PRINTOPTION_CONFIG 0x01

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA STRUCTURES
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct SOMAXDATA
{
   char name[256];
   char sclass[256];
   char version[256];
   char resources[256];
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static struct SOMAXDATA somax_data;

static char loglevel_strings[NUM_SOMAX_LOG_LEVELS+1][SOMAX_LOG_FORMAT_MAX_LENGTH + 1] =
{
   "NULL",
   "INFO   :",
   "WARNING:",
   "ERROR  :",
};

static char mainboardid_strings[SOMAX_NUM_MAINBOARDS+1][SOMAX_MAINBOARDID_STR_MAX_LENGTH + 1] =
{
   "UNKNOWN",
   "EDISON",
   "HIKEY960",
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
somax_mainboard_id_t somax_mainboard_id()
{
   return SOMAX_MAINBOARDID_EDISON;
}

char* somax_mainboard_id_to_string(somax_mainboard_id_t id)
{
   if (id < 0 || id > SOMAX_NUM_MAINBOARDS)
   {
      somax_log_add(SOMAX_LOG_ERR, "somax mainboard id string request for id (%d)out of range", id);
      return mainboardid_strings[SOMAX_MAINBOARDID_UNKNOWN];
   }
   return mainboardid_strings[id];
}


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
   char log_fmt[SOMAX_LOG_FORMAT_MAX_LENGTH + SOMAX_LOG_LEVEL_STRING_MAX_LEN + 1];

   va_list args;
   va_start(args, fmt);
   snprintf(log_fmt, SOMAX_LOG_FORMAT_MAX_LENGTH + SOMAX_LOG_LEVEL_STRING_MAX_LEN + 1, "%s %s\n", loglevel_strings[log_level], fmt);
   vprintf(log_fmt, args);
   va_end(args);
}

float somax_convert_celsius_to_farenheit(float temp_celsius)
{
   return (temp_celsius * (9.0 / 5.0)) + 32.0;
}

float somax_convert_farenheit_to_celsius(float temp_farenheit)
{
   return (temp_farenheit - 32.0) * (5.0 / 9.0) + 32;
}

void *somax_malloc(int size_bytes)
{
   return malloc(size_bytes);
}

void somax_free(void *malloced)
{
   free(malloced);
}