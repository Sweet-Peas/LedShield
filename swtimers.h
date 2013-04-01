/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <pontus.oldberg@invector.se> wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * /Pontus Oldberg
 * ----------------------------------------------------------------------------
 */

#ifndef SWTIMERS_H_INCLUDED
#define SWTIMERS_H_INCLUDED

#define NUMBER_OF_SWTIMERS  16

/*
 * Do not change TMR_RUNNING to any other value that 0.
 * Doing so requires you to rewrite the __interrupt timer logic as well.
 * The other values may be changed and arranged in other ways.
 */
enum timer_stat {
  TMR_RUNNING = 0x00,
  TMR_FREE,
  TMR_ALLOCATED,
  TMR_STOPPED,
  TMR_KICK,
  TMR_ENDED,
  TMR_ERROR,
};

struct kicker {
  struct pt pt;
};

typedef void (*timer_cb)(u8_t timer);
typedef unsigned long timer_time_t;
//typedef unsigned int timer_time_t;

extern struct kicker kicker;
extern timer_time_t swtimer[NUMBER_OF_SWTIMERS];
extern u8_t  timer_table[NUMBER_OF_SWTIMERS];

void init_swtimers(void) ;
void set_timer(u8_t timer, timer_time_t time, timer_cb cb) ;
void set_timer_cnt(u8_t timer, timer_time_t time) ;
timer_time_t get_timer(u8_t timer) ;
u8_t get_timer_status(u8_t timer) ;
char alloc_timer(void) ;
u8_t free_timer(u8_t timer) ;
void stop_timer(u8_t timer) ;
void start_timer(u8_t timer) ;
void init_kicker(void) ;
PT_THREAD(handle_kicker(struct kicker *kick) );

/*
 * Usefull timer macros
 */
#define PT_DELAY(instance,timeout)                   \
  do {                                               \
    set_timer (instance->tmr, timeout, 0);           \
    PT_WAIT_UNTIL(&instance->pt, get_timer(instance->tmr) == 0); \
  } while (0)

#endif // SWTIMERS_H_INCLUDED
