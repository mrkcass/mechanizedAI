#ifndef __somax_h__
#define __somax_h__

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



#define SOMAX_PRINTOPTION_CONFIG 0x01

void somax_data_init(const char name[], const char sclass[], const char version[], const char resources[]);

void somax_print_config(int print_options);

bool somax_commandline_has_option(int argc, char *argv[], const char option_name[]);
bool somax_commandline_options_handler(int argc, char *argv[]);

#endif