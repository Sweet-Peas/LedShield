/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <pontus.oldberg@invector.se> wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * /Pontus Oldberg
 * ----------------------------------------------------------------------------
 */

#include "ramp.h"

/* Enable this when running the debugger */
#define DEBUG
#define MIN_PERCENTAGE_VALUE   0
/* This is a 5 minute delay (5 * 60 * 100) */
#define FIXED_ON_TIME          30000
#define LOW_LIGHT_LEVEL        20
#define HIGH_LIGHT_LEVEL       255

static signal_t ramp_sig;
/* Locally held copy of the ramp instance */
static pt_ramp *lc_ramp;

PT_FACTORY_INIT_START (ramp)
{
    ramp->intensity = 0;
    ramp->dir = 1;
    ramp->state = RAMP_DORMANT;
    ramp_sig = 0;
    lc_ramp = ramp;
}
PT_FACTORY_INIT_END(ramp)

/*
 * Use this function to trigger the ramp
 */
void ramp_trigger (u8_t channel)
{
  Serial.print ("Ramp trigger on channel ");
  Serial.println (channel);
  
  switch (channel)
  {
    case 0:
      /* Channel 0 can not override channel 1 */
      if (lc_ramp->state == RAMP_DORMANT || lc_ramp->state == RAMP_1) {
        lc_ramp->state = RAMP_1;
        lc_ramp->maxlight = LOW_LIGHT_LEVEL;
        ramp_sig = 1;
      } else if (lc_ramp->state == RAMP_2) {
        ramp_sig = 1;
      }
      break;
      
    case 1:
      lc_ramp->maxlight = HIGH_LIGHT_LEVEL;
//      lc_ramp->state = RAMP_2;
      ramp_sig = 2;
      break;
      
    default:
      break;
  }
}

PT_THREAD(thread_ramp(pt_ramp *ramp))
{
    PT_BEGIN (&ramp->pt);

    ramp->tmr = alloc_timer();
    analogWrite (ledPin, 0);

    while (1) {
        /* Wait for a ramp trigger signal */
        PT_WAIT_UNTIL (&ramp->pt, ramp_sig);
        /* Reset the signal */
        ramp_sig = 0;
        ramp->cycle = 1;
        Serial.println ("Starting to ramp !");

        while (ramp->cycle) {
do_continue_ramp:
            /* Set light intensity */
            analogWrite (ledPin, ramp->intensity);

            /* Wait a while */
#ifdef DEBUG
            ramp->ramptime = (uint32_t)1;
#else
            ramp->ramptime = (uint32_t)get_pot (RAMP_TIME) / 50;
            if (!ramp->ramptime)
                ramp->ramptime = 1;
#endif
            set_timer(ramp->tmr, ramp->ramptime, 0);
            PT_WAIT_UNTIL (&ramp->pt, (get_timer(ramp->tmr) == 0) || ramp_sig);
            if (ramp_sig) {
                ramp_sig = 0;
//                ramp->dir = 1;
            }
            /* Adjust intensity */
            ramp->intensity += ramp->dir;

            if ((ramp->intensity > ramp->maxlight) && (ramp->dir > 0)) {
                ramp->intensity = ramp->maxlight;
                if (ramp->intensity < 0) ramp->intensity = 0;
                ramp->dir = -1;
do_again:
#ifdef DEBUG
                Serial.print ("Reached hold light ");
                Serial.println (ramp->intensity);
                
                /* Fake ontime for testing */
                ramp->ontime = (int32_t)FIXED_ON_TIME;
#else
                /* On time is between aprox 1-5 minutes */
                ramp->ontime = (uint32_t)get_pot (ON_TIME) * 44 + 7500;
#endif
                set_timer(ramp->tmr, ramp->ontime, 0);
do_again_no_reset:
                PT_WAIT_UNTIL (&ramp->pt, (get_timer (ramp->tmr) == 0) || ramp_sig);

                Serial.print (ramp->state);
                Serial.print (", ");
                Serial.println (ramp_sig);
                
                /* Check if we received a pir trigger while waiting */
                if (ramp->state == RAMP_1 && ramp_sig == 1) {
                    Serial.println ("1");
                    ramp_sig = 0;
                    goto do_again;
                } else if (ramp->state == RAMP_1 && ramp_sig == 2) {
                    Serial.println ("2");
                    ramp->state = RAMP_2;
                    ramp->dir = 1;
                    ramp_sig = 0;
                    goto do_continue_ramp;
                } else if (ramp->state == RAMP_1 && ramp_sig != 0) {
                    Serial.println ("3");
                    ramp_sig = 0;
                    goto do_again;
                } else if (ramp->state == RAMP_2 && ramp_sig == 2) {
                    Serial.println ("4");
                    ramp->dir = -1;
                    ramp->target_intensity = MIN_PERCENTAGE_VALUE;
                    ramp->intensity += ramp->dir;
                    ramp_sig = 0;
                    goto do_continue_ramp;
                } else if (ramp->state == RAMP_2 && ramp_sig == 1) {
                    ramp_sig = 0;
                    goto do_again_no_reset;
                }
                
                /* If the pir sensor is still high we stay here */
                if (get_current_pir_state(0) || get_current_pir_state(1)) {
                  Serial.println ("PIR sensor is high so continue");
                  goto do_again;
                }
#ifdef DEBUG
                Serial.println ("Starting ramping down.");
#endif
            } else if ((ramp->intensity <= ramp->target_intensity)) {
                if (ramp->target_intensity == 0)
                    ramp->intensity = 0;
                analogWrite (ledPin, ramp->intensity);
                ramp->dir = 1;
                ramp_sig = 0;
                /* stop the cycle */
                ramp->cycle = 0;
            }
        }
        ramp->state = RAMP_DORMANT;
        Serial.println ("Ramp done.");
    }
    
    PT_END (&ramp->pt);
}
