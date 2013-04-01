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
 * This is the thread that reads the PIR sensors
 * and inserts pir events into the event framework.
 */

/* Local table of pir instances */
pt_pir_t *pir_tab[2];

void pir_init(pt_pir_t *self, u8_t channel, int pin)
{
  PT_INIT (&self->pt);
  self->pin = pin;
  self->channel = channel;
  pinMode (self->pin, INPUT);
  pir_tab[channel] = self;
}

/*
 * This function returns the state of a pir channel
 */
u8_t get_current_pir_state(u8_t channel)
{
  pt_pir_t *lpir;

  if (channel >= NUM_PIR_CHANNELS)
    return 0xff;

  lpir = pir_tab[channel];
  return digitalRead (lpir->pin);
}
/*
 * The protothread for the pir handler-
 */
PT_THREAD (handle_pir (pt_pir_t *self))
{
  PT_BEGIN(&self->pt);
  
  self->tmr = alloc_timer();
  
  /*
   * Wait for the PIR sensor to return to inactive state.
   * This is necessary as the sensor is active directly after
   * power on.
   */
  PT_WAIT_WHILE (&self->pt, digitalRead (self->pin));
  Serial.println ("PIR Sensor up and running !");
  
  while (1)
  {
    /* Wait for the PIR sensor to trigger */
    PT_WAIT_UNTIL (&self->pt, digitalRead (self->pin));
    Serial.print ("Got PIR trigger on channel ");
    Serial.println (self->channel);
    ramp_trigger (self->channel);
    /* Wait for the trigger to go inactive again */
    PT_WAIT_WHILE (&self->pt, digitalRead (self->pin));
    Serial.print ("Channel ");
    Serial.print (self->channel);
    Serial.println (" trigger state returned to non triggered.");
  }
  PT_END(&self->pt);
}

