/*
 * LedShield
 *
 * This is a LED lighting controller with advanced control functions.
 *
 */
#include <TimerOne.h>
#include <RemoteReceiver.h>

#include "pt.h"
#include "ptfactory.h"
#include "types.h"
#include "swtimers.h"
#include "pir.h"
#include "ramp.h"
#include "light.h"

/*
 * Enable this flag if you want to have debug print output
 */
#define DEBUG

/* This is the valid period time for this system */
#define VALID_PERIOD         432
#define RF_CODE_HEARTBEAT    11234
#define RF_CODE_ROOM_TRIGGER 1536

/*
 * This is a timer for detecting if the RF PIR transmitter have stopped
 * sending heartbeat messages. The heartbeat message comes every 15 seconds
 * from the PIR RF transmitter, if we have a timeout of 1 minute we can
 * with a high amount of certainty determine whether the unit have stopped
 * transmitting or not.
 */
unsigned long  pir_heartbeat_timer;
/* Timeout is currently set to 1 minute */
#define   PIR_HEARTBEAT_TIMER    60000UL

int ledPin = 6;     // LED light connected to digital pin 6
int lightPin1 = A0; // Light sensor pin connected here
int alarmPin = 7;   // An alarm pin that can be connected to a LED
                    // Active low
int whiteLed = 13;

/*
 * Pins that are connected to the PIR sensors.
 *
 * Pir 1 is considered the room sensor. It will detect if a person
 * has entered the room and gently light the light. The Pir 1 sensor
 * can also be a RF based PIR sensor. In this case a different mechanism
 * is used to trigger the light.
 *
 * Pir 2 is the close proximity sensor, it shall be set to the minimum
 * sensitivity and placed downwards where you want to have the activation
 * area.
 */
int pin_pir1 = 8;
int pin_pir2 = 7;

/* Protothread instances */
pt_pir_t   pir_1;
pt_pir_t   pir_2;
pt_light_t light_1;
pt_ramp    ramp;

void setup()  {
  Serial.begin(115200);
  /* Setup pins */
  pinMode (ledPin, OUTPUT);
  analogWrite (ledPin, 100);
  pinMode (alarmPin, OUTPUT);
  digitalWrite (alarmPin, HIGH);
  pinMode (whiteLed, OUTPUT);
  digitalWrite (whiteLed, LOW);

  // Framework initialization
  init_swtimers();

  // Initialize protothreads
  light_init(&light_1, 0, lightPin1);
  pir_init(&pir_1, 0, pin_pir1);
  pir_init(&pir_2, 1, pin_pir2);
  init_ramp(&ramp);

  /* Use pin 3 (interrupt 1) as RF data input */
  RemoteReceiver::init(1 ,3 , incomingRFMessage);
  pir_heartbeat_timer = millis() + PIR_HEARTBEAT_TIMER;

  // Blink once
  digitalWrite (whiteLed, HIGH);
  delay (500);
  digitalWrite (whiteLed, LOW);

  Serial.println (F("Initialization done !"));
}

void loop()  {
  handle_light (&light_1);
  handle_pir (&pir_1);
  handle_pir (&pir_2);
  thread_ramp (&ramp);
  
  /* Check for any alarm conditions */
  if (millis() > pir_heartbeat_timer) {
    // Not much we can do but to turn the alarm LED on....
    digitalWrite (alarmPin, LOW);
    digitalWrite (whiteLed, HIGH);  // Just for show
  }
}

void incomingRFMessage(unsigned long receivedCode, unsigned int period) 
{
  // First make sure that the period duration is within limits
  if (period > (VALID_PERIOD - 2) && period < (VALID_PERIOD + 2)) {
    switch (receivedCode) {
#if defined(DEBUG)      
      Serial.print(", Code: ");
      Serial.print(receivedCode);
      Serial.print(", period duration: ");
      Serial.print(period);
      Serial.println("us.");
#endif
      case RF_CODE_HEARTBEAT:
        // Reset the timer
        pir_heartbeat_timer = millis() + PIR_HEARTBEAT_TIMER;
        break;
        
      case RF_CODE_ROOM_TRIGGER:
        // Trigger channel 0, which is the room trigger
        ramp_trigger (0, 0);
        break;
        
      default:
        // Not yet supported code received
        break;
    }
  }
}

