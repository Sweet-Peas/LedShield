/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <pontus.oldberg@invector.se> wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * /Pontus Oldberg
 * ----------------------------------------------------------------------------
 */

#ifndef TEST_H
#define	TEST_H

/* ADC channels associations */
#define RAMP_TIME   0
#define MAX_LIGHT   1
#define ON_TIME     2

enum {
  RAMP_NOT_USED,
  RAMP_DORMANT,
  RAMP_1,
  RAMP_2,
};

PT_INTERFACE_START(ramp);
    uint8_t tmr;              /* Timer */
    uint8_t state;            /* Determines the current state */
    int16_t intensity;        /* Current intensity */
    int16_t target_intensity; /* The current target intensity */
    int16_t dir;              /* Direction to step */
    int16_t step;             /* Value to step */
    int16_t maxlight;         /* Temporary storage */
    uint32_t ontime;          /* The time the light is on */
    uint32_t ramptime;        /* Time between ticks */
    uint8_t cycle;            /* */
PT_INTERFACE_END(ramp);

PT_THREAD(thread_ramp(pt_ramp *ta));
void ramp_trigger (u8_t channel);
#endif	/* TEST_H */

