#ifndef __gpio_interface_h__
#define __gpio_interface_h__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// author: mark cass
// project: somax personal AI
// project url: https://mechanizedai.com
// license: open source and free for all uses without encumbrance.
//
// FILE: gpio_interface.h
// DESCRIPTION: Interface for general purpose input output communication.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include "somax.h"


#if SOMAX_MAINBOARDID == SOMAX_MAINBOARDID_EDISON
#define GPIO_CONTROLLERID_MAINBOARD 1
#define GPIO_NUM_CONTROLLERID       1
#elif SOMAX_MAINBOARDID == SOMAX_MAINBOARDID_HIKEY960
#define GPIO_CONTROLLERID_MAINBOARD 1
#define GPIO_CONTROLLERID_TEENSY    2
#define GPIO_NUM_CONTROLLERID       2
#endif

#define GPIO_PINID_0    0
#define GPIO_PINID_1    1
#define GPIO_PINID_2    2
#define GPIO_PINID_3    3
#define GPIO_PINID_4    4
#define GPIO_PINID_5    5
#define GPIO_PINID_6    6
#define GPIO_PINID_7    7
#define GPIO_PINID_8    8
#define GPIO_PINID_9    9
#define GPIO_PINID_10   10
#define GPIO_PINID_11   11
#define GPIO_PINID_12   12
#define GPIO_PINID_13   13
#define GPIO_PINID_14   14
#define GPIO_PINID_15   15
#define GPIO_PINID_16   16
#define GPIO_PINID_17   17
#define GPIO_PINID_18   18
#define GPIO_PINID_19   19
#define GPIO_PINID_20   20
#define GPIO_PINID_21   21
#define GPIO_PINID_22   22
#define GPIO_PINID_23   23
#define GPIO_PINID_24   24
#define GPIO_PINID_25   25
#define GPIO_PINID_26   26
#define GPIO_PINID_27   27
#define GPIO_PINID_28   28
#define GPIO_PINID_29   29
#define GPIO_PINID_30   30
#define GPIO_PINID_31   31
#define GPIO_PINID_32   32
#define GPIO_PINID_33   33
#define GPIO_PINID_34   34
#define GPIO_PINID_35   35
#define GPIO_PINID_36   36
#define GPIO_PINID_37   37
#define GPIO_PINID_38   38
#define GPIO_PINID_39   39

#define GPIO_PULLTYPE_UP     0
#define GPIO_PULLTYPE_DOWN   1

#define GPIO_DIRECTION_IN  0
#define GPIO_DIRECTION_OUT 1

#define GPIO_STATE_LO   0
#define GPIO_STATE_HI   1

#define GPIO_MAX_CONTEXTS 64
struct GPIO_CONTEXT;
typedef GPIO_CONTEXT *gpio_context;

typedef int gpio_controllerid;
typedef int gpio_pinid;
typedef smx_byte gpio_direction;
typedef smx_byte gpio_pulltype;
typedef smx_byte gpio_state;

gpio_context gpio_ini_open(gpio_controllerid controller_id, gpio_pinid pin_id, gpio_direction direction);
void gpio_ini_close(gpio_context gpio);

void gpio_cfg_direction(gpio_context gpio, gpio_direction direction);
void gpio_cfg_pull(gpio_context gpio, gpio_pulltype strength);

gpio_state gpio_inf_state(gpio_context gpio);
void gpio_cfg_state(gpio_context gpio, gpio_state state);

#endif