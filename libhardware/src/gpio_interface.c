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
#include <stdio.h>
#include <string.h>

#include "mraa.h"
#include "mraa/gpio.h"

#include "somax.h"
#include "gpio_interface.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//CONSTANTS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define TEST_REG_ERROR -1

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//FUNCTION DECLARATIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
extern char *decode_error(mraa_result_t code);

static bool gpio_init_controller(gpio_controllerid controller_id);

static gpio_context edison_init_pin(gpio_pinid pin_id, gpio_direction direction);
static void edison_deinit_pin(gpio_context gpio);
static void edison_cfg_direction(gpio_context gpio, gpio_direction direction);
static void edison_cfg_pull(gpio_context gpio, gpio_pulltype strength);
static gpio_state edison_inf_state(gpio_context gpio);
static void edison_cfg_state(gpio_context gpio, gpio_state state);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA STRUCTURES
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

struct GPIO_CONTEXT
{
   gpio_pinid pin_id;
   gpio_controllerid controller_id;
   gpio_direction direction;
   int context_slot;
};

typedef gpio_context (*controller_init_pin)(gpio_pinid pin_id, gpio_direction direction);
typedef void (*controller_deinit_pin)(gpio_context gpio);
typedef void (*controller_cfg_direction)(gpio_context gpio, gpio_direction direction);
typedef void (*controller_cfg_pull)(gpio_context gpio, gpio_pulltype strength);
typedef gpio_state (*controller_inf_state)(gpio_context gpio);
typedef void (*controller_cfg_state)(gpio_context gpio, gpio_state state);

struct GPIO_CONTROLLER_FUNCS
{
   controller_init_pin        ini_init;
   controller_deinit_pin      ini_deinit;
   controller_cfg_direction   cfg_direction;
   controller_cfg_pull        cfg_pull;
   controller_inf_state       inf_state;
   controller_cfg_state       cfg_state;
};

struct GPIO_CONTROLLER
{
   struct GPIO_CONTEXT contexts[GPIO_MAX_CONTEXTS];
   int context_count;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static struct GPIO_CONTROLLER gpio_controller[GPIO_NUM_CONTROLLERID+1];
static struct GPIO_CONTROLLER_FUNCS gpio_controller_ops[GPIO_NUM_CONTROLLERID+1] =
{
   {0,0,0,0,0},
   #if SOMAX_MAINBOARDID == SOMAX_MAINBOARDID_EDISON
   {
      edison_init_pin,
      edison_deinit_pin,
      edison_cfg_direction,
      edison_cfg_pull,
      edison_inf_state,
      edison_cfg_state,
   },
   #elif SOMAX_MAINBOARDID == SOMAX_MAINBOARDID_HIKEY960
   {0,0,0,0,0},
   {0,0,0,0,0},
   #endif
};



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

gpio_context gpio_ini_open(gpio_controllerid controller_id, gpio_pinid pin_id, gpio_direction direction)
{
   if (gpio_controller[controller_id].context_count == 0)
      if (!gpio_init_controller(controller_id))
         return 0;

   gpio_context new_ctx = gpio_controller_ops[controller_id].ini_init(pin_id, direction);
   if (new_ctx)
      new_ctx->controller_id = controller_id;
   return new_ctx;
}

void gpio_ini_close(gpio_context gpio)
{
   if (gpio == NULL)
      return;

   if (gpio->controller_id)
   {
      gpio_controller_ops[gpio->controller_id].ini_deinit(gpio);
   }
}

void gpio_cfg_direction(gpio_context gpio, gpio_direction direction)
{
   if (gpio->direction != direction)
      gpio_controller_ops[gpio->controller_id].cfg_direction(gpio, direction);
}

void gpio_cfg_pull(gpio_context gpio, gpio_pulltype strength)
{
   gpio_controller_ops[gpio->controller_id].cfg_pull(gpio, strength);
}

gpio_state gpio_inf_state(gpio_context gpio)
{
   return gpio_controller_ops[gpio->controller_id].inf_state(gpio);
}

void gpio_cfg_state(gpio_context gpio, gpio_state state)
{
   gpio_controller_ops[gpio->controller_id].cfg_state(gpio, state);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool gpio_init_controller(gpio_controllerid controller_id)
{
   return true;
}

#if SOMAX_MAINBOARDID == SOMAX_MAINBOARDID_EDISON
#define GPIO_EDISON_NUM_PINS 16
static int edison_gpio_map[GPIO_EDISON_NUM_PINS] =
{
   130, //spi 0 (edison spi 1) RESET
   128, //spi 0 (edison spi 1) DC (data/command)
};

struct EDISON_GPIO_CONTEXT
{
   mraa_gpio_context pin;
};
typedef struct EDISON_GPIO_CONTEXT* edison_context;
static struct EDISON_GPIO_CONTEXT edison_contexts[GPIO_MAX_CONTEXTS];

static gpio_context edison_init_pin(gpio_pinid pin_id, gpio_direction direction)
{
   int ed_pin = edison_gpio_map[pin_id];
   GPIO_CONTROLLER * controller = &gpio_controller[GPIO_CONTROLLERID_MAINBOARD];

   gpio_context ctx = &controller->contexts[controller->context_count];
   edison_context ed_ctx = &edison_contexts[controller->context_count];

   ctx->context_slot = controller->context_count;
   ed_ctx->pin = mraa_gpio_init_raw(ed_pin);
   mraa_gpio_use_mmaped(ed_ctx->pin, 1);
   edison_cfg_direction(ctx, direction);

   controller->context_count++;
   return ctx;
}

static void edison_deinit_pin(gpio_context gpio)
{
   mraa_gpio_context ed_gpio = edison_contexts[gpio->context_slot].pin;
   GPIO_CONTROLLER *controller = &gpio_controller[GPIO_CONTROLLERID_MAINBOARD];
   mraa_gpio_close(ed_gpio);
   edison_contexts[gpio->context_slot].pin = 0;
   controller->contexts[gpio->context_slot].controller_id = 0;
}

static void edison_cfg_direction(gpio_context gpio, gpio_direction direction)
{
   mraa_gpio_context ed_gpio = edison_contexts[gpio->context_slot].pin;
   mraa_result_t result;
   if (direction == GPIO_DIRECTION_OUT)
      result = mraa_gpio_dir(ed_gpio, MRAA_GPIO_OUT);
   else
      result = mraa_gpio_dir(ed_gpio, MRAA_GPIO_IN);
   if (result != MRAA_SUCCESS)
      somax_log_add(SOMAX_LOG_ERR, "GPIO. cfg_direction. couldn't change direction. pin = %d", gpio->pin_id);
   else
      gpio->direction = direction;
}

static void edison_cfg_pull(gpio_context gpio, gpio_pulltype strength)
{
   //todo: implement edison_cfg_pullup
}

static gpio_state edison_inf_state(gpio_context gpio)
{
   mraa_gpio_context ed_gpio = edison_contexts[gpio->context_slot].pin;
   return mraa_gpio_read(ed_gpio);
}

static void edison_cfg_state(gpio_context gpio, gpio_state state)
{
   mraa_gpio_context ed_gpio = edison_contexts[gpio->context_slot].pin;
   mraa_gpio_write(ed_gpio, state);
}
#endif