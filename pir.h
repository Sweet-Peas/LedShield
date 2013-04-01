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
 * Interface for pir
 */
#define NUM_PIR_CHANNELS 2

#include "types.h"
#include "pt.h"

struct _pt_pir {
  struct pt pt;
  u8_t tmr;
  u8_t channel;
  int pin;
};

typedef struct _pt_pir pt_pir_t;

PT_THREAD (handle_pir (pt_pir_t *self));
u8_t get_current_pir_state(u8_t channel);
