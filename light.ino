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
 * This is the thread that reads the light value.
 */

/* Local table of light instances */
pt_light_t *light_tab[NUM_LIGHT_CHANNELS];

void light_init(pt_light_t *self, u8_t channel, int pin)
{
  PT_INIT (&self->pt);
  self->pin = pin;
  self->channel = channel;
  light_tab[channel] = self;
}

/*
 * This function returns the state of a pir channel
 */
int get_current_light(u8_t channel)
{
  pt_light_t *llight;

  if (channel >= NUM_PIR_CHANNELS)
    return -1;

  llight = light_tab[channel];
  return analogRead (llight->pin);
}

/*
 * The protothread for the pir handler-
 */
PT_THREAD (handle_light (pt_light_t *self))
{
  PT_BEGIN(&self->pt);
  
  self->tmr = alloc_timer();
  Serial.println (F("Light Sensor up and running !"));
  
  while (1)
  {
    self->light = 1;
    PT_DELAY (self, 10);
  }
  PT_END(&self->pt);
}

