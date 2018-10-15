#ifndef __somax_h__
#define __somax_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: somax.h
// DESCRIPTION: Somax constants and utility functions.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// #ifndef SOMAX_NAME
// #define SOMAX_NAME "UNKNOWN"
// #endif
// #ifndef SOMAX_CLASS
// #define SOMAX_CLASS "UNKNOWN"
// #endif
// #ifndef SOMAX_VERSION
// #define SOMAX_VERSION "UNKNOWN"
// #endif
// #ifndef SOMAX_RESOURCES
// #define SOMAX_RESOURCES "NONE"
// #endif

//microseconds per millisecond
#define U_MILLISECOND 1000

#define SOMAX_PRINTOPTION_CONFIG 0x01

#define MAX_STR_DEVICEID_LEN 32

void somax_data_init(const char name[], const char sclass[], const char version[], const char resources[]);

void somax_print_config(int print_options);

bool somax_commandline_has_option(int argc, char *argv[], const char option_name[]);
bool somax_commandline_options_handler(int argc, char *argv[]);

typedef int somax_mainboard_id_t;
#define SOMAX_NUM_MAINBOARDS           2
#define SOMAX_MAINBOARDID_UNKNOWN      0
#define SOMAX_MAINBOARDID_EDISON       1
#define SOMAX_MAINBOARDID_HIKEY960     2
#define SOMAX_MAINBOARDID_STR_MAX_LENGTH 16
somax_mainboard_id_t somax_mainboard_id();
char* somax_mainboard_id_to_string(somax_mainboard_id_t id);

#define NUM_SOMAX_LOG_LEVELS  3
#define SOMAX_LOG_INFO        1
#define SOMAX_LOG_WARN        2
#define SOMAX_LOG_ERR         3

//not including NULL terminator
#define SOMAX_LOG_FORMAT_MAX_LENGTH 256
//not including NULL terminator
#define SOMAX_LOG_LEVEL_STRING_MAX_LEN 8

void somax_log_add(int log_level, const char *fmt, ...);

float somax_convert_celsius_to_farenheit(float temp_celsius);
float somax_convert_farenheit_to_celsius(float temp_farenheit);

void* somax_malloc(int size_bytes);
void somax_free(void * malloced);

#endif