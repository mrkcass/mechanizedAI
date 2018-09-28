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

//microseconds per millisecond
#define U_MILLISECOND 1000

#define SOMAX_PRINTOPTION_CONFIG 0x01

void somax_data_init(const char name[], const char sclass[], const char version[], const char resources[]);

void somax_print_config(int print_options);

bool somax_commandline_has_option(int argc, char *argv[], const char option_name[]);
bool somax_commandline_options_handler(int argc, char *argv[]);

#define SOMAX_LOG_INFO 1
#define SOMAX_LOG_WARN 2
#define SOMAX_LOG_ERR  3

void somax_log_add(int log_level, const char *fmt, ...);

#endif