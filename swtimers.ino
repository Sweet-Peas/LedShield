/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <pontus.oldberg@invector.se> wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * /Pontus Oldberg
 * ----------------------------------------------------------------------------
 */

#define A_(x)
#define B_(x)
#define C_(x)

timer_time_t swtimer[NUMBER_OF_SWTIMERS];
u8_t  timer_table[NUMBER_OF_SWTIMERS];
timer_cb timer_cbs[NUMBER_OF_SWTIMERS];

struct kicker kicker;

u8_t callback_kicker;
static u8_t count = 0;
static u8_t tmcnt;


/*
 * Initialize software timers
 * This function must be executed before timer 0 interrupts
 * are enabled.
 */
void init_swtimers(void)
{
  u8_t i;

  for (i=0 ; i<NUMBER_OF_SWTIMERS ; i++)
  {
    /* Setting the timer value to 0 indicates that it is inactive and
     * not counting */
    timer_table[i] = TMR_FREE;
    swtimer[i]   = 0x0000;
    timer_cbs[i] = 0x0000;
  }

  PT_INIT(&kicker.pt);
  
  Timer1.initialize (10000);
  Timer1.attachInterrupt (clockISR);
}

void clockISR(void)
{
  /* Count all used sw timers down by one */
  for(tmcnt=0 ; tmcnt<NUMBER_OF_SWTIMERS ; tmcnt++)
  {
    if (!timer_table[tmcnt])
    {
      swtimer[tmcnt]--;
      if (!swtimer[tmcnt])
      {
        if (timer_cbs[tmcnt])
        {
          timer_table[tmcnt] = TMR_KICK;
          callback_kicker = 1;
        }
        else
        {
          timer_table[tmcnt] = TMR_ENDED;
        }
      }
    }
  }
}

/*
 * Allocate a timer, can range from 0-127.
 * Negative number are error codes.
 */
char alloc_timer(void)
{
  u8_t i;
#ifdef DEBUG_SWTIMERS
  u8_t j;
#endif
//  ET0 = INTERRUPT_OFF;
  for (i=0 ; i<NUMBER_OF_SWTIMERS ; i++)
  {
    if (timer_table[i] == TMR_FREE) {
      /* Timer is now taken */
      timer_table[i] = TMR_ALLOCATED;
      swtimer[i]   = 0x0000;
      timer_cbs[i] = 0x0000;
      // ET0 = INTERRUPT_ON;
      count++;
      A_(printf("Allocating Timer %d, Timers used = %d '", i, count);)
#ifdef DEBUG_SWTIMERS
      for (j=0 ; j<NUMBER_OF_SWTIMERS ; j++) {
        switch (timer_table[j])
        {
          case TMR_RUNNING:
            putchar('R');
            break;
          case TMR_FREE:
            putchar('F');
            break;
          case TMR_ALLOCATED:
            putchar('A');
            break;
          case TMR_STOPPED:
            putchar('S');
            break;
          case TMR_KICK:
            putchar('K');
            break;
          case TMR_ENDED:
            putchar('E');
            break;
          default:
            putchar('U');
            break;
        }
      }
#endif
      A_(printf("\r\n");)
      return i;
    }
  }
  // ET0 = INTERRUPT_ON;
  return -1;
}

/*
 * Free a timer
 */
u8_t free_timer(u8_t timer)
{
#ifdef DEBUG_SWTIMERS
  u8_t j;
#endif
  if (timer >= NUMBER_OF_SWTIMERS) {
    A_(printf (__AT__ " Requested timer exceeded max number of timers !\n");)
    return 0;
  }

  // ET0 = INTERRUPT_OFF;
  if (timer_table[timer] != TMR_FREE) {
    timer_table[timer] = TMR_FREE;
    count--;
    A_(printf("Freeing Timer %d, Timers used = %d ", timer, count);)
#ifdef DEBUG_SWTIMERS
    for (j=0 ; j<NUMBER_OF_SWTIMERS ; j++) {
      switch (timer_table[j])
      {
        case TMR_RUNNING:
          putchar('R');
          break;
        case TMR_FREE:
          putchar('F');
          break;
        case TMR_ALLOCATED:
          putchar('A');
          break;
        case TMR_STOPPED:
          putchar('S');
          break;
        case TMR_KICK:
          putchar('K');
          break;
        case TMR_ENDED:
          putchar('E');
          break;
        default:
          putchar('U');
          break;
      }
    }
#endif
    A_(printf("\r\n");)
  }
  // ET0 = INTERRUPT_ON;
  return 0;
}

/*
 * Set a timer to a defined value with callback.
 * If callbacks are not to be used, this must be set to NULL
 * The timer is started immediatly before returning
 */
void set_timer(u8_t timer, timer_time_t time, timer_cb cb)
{
  if (timer >= NUMBER_OF_SWTIMERS) {
    A_(printf (__AT__ " Requested timer exceeded max number of timers !\n");)
    return;
  }

  // ET0 = INTERRUPT_OFF;
  swtimer[timer] = time;
  timer_cbs[timer] = cb;
  timer_table[timer] = TMR_RUNNING;
  // ET0 = INTERRUPT_ON;
}

/*
 * Renews the timer counter
 */
void set_timer_cnt(u8_t timer, timer_time_t time)
{
  if (timer >= NUMBER_OF_SWTIMERS) {
    A_(printf (__AT__ " Requested timer exceeded max number of timers !\n");)
    return;
  }
  // ET0 = INTERRUPT_OFF;
  swtimer[timer] = time;
  // ET0 = INTERRUPT_ON;
}

/*
 * Get the value of a specific timer
 */
timer_time_t get_timer(u8_t timer)
{
  timer_time_t value;

  if (timer >= NUMBER_OF_SWTIMERS) {
    A_(printf (__AT__ " Requested timer exceeded max number of timers !\n");)
    return 0;
  }

  // ET0 = INTERRUPT_OFF;
  value = swtimer[timer];
  // ET0 = INTERRUPT_ON;

  return value;
}

/*
 * Get the status of a specified timer
 */
u8_t get_timer_status(u8_t timer)
{
  if (timer >= NUMBER_OF_SWTIMERS) {
    A_(printf (__AT__ " Requested timer exceeded max number of timers !\n");)
    return TMR_ERROR;
  }
  return timer_table[timer];
}

/*
 * Stop the timer from counting down
 */
void stop_timer(u8_t timer)
{
  if (timer >= NUMBER_OF_SWTIMERS) {
    A_(printf (__AT__ " Requested timer exceeded max number of timers !\n");)
    return;
  }

  // ET0 = INTERRUPT_OFF;
  timer_table[timer] = TMR_STOPPED;
  // ET0 = INTERRUPT_ON;
}

/*
 * Start the timer counting
 */
void start_timer(u8_t timer)
{
  if (timer >= NUMBER_OF_SWTIMERS) {
    A_(printf (__AT__ " Requested timer exceeded max number of timers !\n");)
    return;
  }

  // ET0 = INTERRUPT_OFF;
  timer_table[timer] = TMR_RUNNING;
  // ET0 = INTERRUPT_ON;
}

/**
 * This thread listens to the <callback_kicker> signal and when it is
 * triggered it goes through all software timers and executes the
 * indicated call back methods.
 * This functionality was broken out of the timer 0 __interrupt routine
 * as it was severely instable.
 */
PT_THREAD(handle_kicker(struct kicker *Kicker) )
{
  PT_BEGIN(&Kicker->pt);

  while (1)
  {
    /* Wait to be kicked by the timer system */
    PT_WAIT_UNTIL(&Kicker->pt, (callback_kicker));

    // ET0 = INTERRUPT_OFF;
    callback_kicker = 0;
    {
      u8_t i;

      for (i=0 ; i<NUMBER_OF_SWTIMERS ; i++)
      {
        if (timer_table[i] == TMR_KICK)
        {
          /* Allow timer 0 interrupts while executing callback */
          // ET0 = INTERRUPT_ON;
          timer_cbs[i](i);
          // ET0 = INTERRUPT_OFF;
        }
      }
    }
    // ET0 = INTERRUPT_ON;
  }
  PT_END(&Kicker->pt);
}

/* End of file */
