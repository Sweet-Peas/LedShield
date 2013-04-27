/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <pontus.oldberg@invector.se> wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * /Pontus Oldberg
 * ----------------------------------------------------------------------------
 */

/*
 * Interface for light
 */
#define NUM_LIGHT_CHANNELS 1

#include "types.h"
#include "pt.h"

struct _pt_light {
  struct pt pt;
  u8_t tmr;
  u8_t channel;
  int pin;
  long light;
};

typedef struct _pt_light pt_light_t;

PT_THREAD (handle_light (pt_light_t *self));
int get_current_light(u8_t channel);
