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

static void edison_ini_pin(gpio_context pin);
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
   mraa_gpio_context pin;
   gpio_pinid pin_id;
   gpio_controllerid controller_id;
   gpio_direction direction;
};

typedef void (*controller_init_pin)(gpio_context pin);
typedef void (*controller_cfg_direction)(gpio_context gpio, gpio_direction direction);
typedef void (*controller_cfg_pull)(gpio_context gpio, gpio_pulltype strength);
typedef gpio_state (*controller_inf_state)(gpio_context gpio);
typedef void (*controller_cfg_state)(gpio_context gpio, gpio_state state);

struct GPIO_CONTROLLER
{
   controller_init_pin        ini_pin;
   controller_cfg_direction   cfg_direction;
   controller_cfg_pull        cfg_pull;
   controller_inf_state       inf_state;
   controller_cfg_state       cfg_state;
   int context_count;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DATA
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static struct GPIO_CONTEXT gpio_contexts[GPIO_MAX_CONTEXTS];
static int gpio_contexts_used;

static struct GPIO_CONTROLLER gpio_controller[GPIO_NUM_CONTROLLERID+1] =
{
   {0,0},
   {
      edison_ini_pin,
      edison_cfg_direction,
      edison_cfg_pull,
      edison_inf_state,
      edison_cfg_state,
      0,
   },
   {
      0,0,0,0,0,
      0,
   }
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

   gpio_context new_ctx = &gpio_contexts[gpio_contexts_used];

   new_ctx->controller_id = controller_id;
   new_ctx->pin_id = pin_id;
   new_ctx->direction = direction;
   gpio_controller[new_ctx->controller_id].ini_pin(new_ctx);
   gpio_controller[new_ctx->controller_id].cfg_direction(new_ctx, direction);

   gpio_controller[controller_id].context_count++;
   gpio_contexts_used++;

   return new_ctx;
}

void gpio_ini_close(gpio_context gpio)
{
   if (gpio == NULL)
      return;

   if (gpio->controller_id)
   {
      gpio_controller[gpio->controller_id].context_count--;
      gpio_contexts_used--;
      memset(gpio, 0, sizeof(struct GPIO_CONTEXT));
   }
}

void gpio_cfg_direction(gpio_context gpio, gpio_direction direction)
{
   if (gpio->direction != direction)
      gpio_controller[gpio->controller_id].cfg_direction(gpio, direction);
}

void gpio_cfg_pull(gpio_context gpio, gpio_pulltype strength)
{
   gpio_controller[gpio->controller_id].cfg_pull(gpio, strength);
}

gpio_state gpio_inf_state(gpio_context gpio)
{
   return gpio_controller[gpio->controller_id].inf_state(gpio);
}

void gpio_cfg_state(gpio_context gpio, gpio_state state)
{
   gpio_controller[gpio->controller_id].cfg_state(gpio, state);
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

#define GPIO_EDISON_NUM_PINS 16
static int edison_gpio_map[GPIO_EDISON_NUM_PINS] =
{
   130,
};

static void edison_ini_pin(gpio_context ctx)
{
   int ed_pin = edison_gpio_map[ctx->pin_id];
   ctx->pin = mraa_gpio_init_raw(ed_pin);
   mraa_gpio_use_mmaped(ctx->pin, 1);
}

static void edison_cfg_direction(gpio_context ctx, gpio_direction direction)
{
   mraa_result_t result;
   if (direction == GPIO_DIRECTION_OUT)
      result = mraa_gpio_dir(ctx->pin, MRAA_GPIO_OUT);
   else
      result = mraa_gpio_dir(ctx->pin, MRAA_GPIO_IN);
   if (result != MRAA_SUCCESS)
      somax_log_add(SOMAX_LOG_ERR, "GPIO. cfg_direction. couldn't change direction. pin = %d", ctx->pin_id);
}

static void edison_cfg_pull(gpio_context gpio, gpio_pulltype strength)
{
   //todo: implement edison_cfg_pullup
}

static gpio_state edison_inf_state(gpio_context gpio)
{
   return mraa_gpio_read(gpio->pin);
}

static void edison_cfg_state(gpio_context gpio, gpio_state state)
{
   mraa_gpio_write(gpio->pin, state);
}