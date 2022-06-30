#include <Arduino.h>
#include "functions.h"
#include "meranie.h"
#include "ISRs.h"
#include <avr/wdt.h>


void tg1_increment(){
  ++tg1_rot;
}
void tg2_increment(){
  ++tg2_rot;
}

float vykon1; //kW
float vykon2;
float cos1;
float cos2;

void calculatePower(){
  float cP1avg = 0;
  float cP2avg = 0;
  float jP1avg = 0;
  float jP2avg = 0;
//                             U      I

// 2.72 = U  0.2639 = I  f = 3  1000W = 1kW
  if(cC1) cP1avg = (float)cP1/cC1*0.001504;
  if(cC2) cP2avg = (float)cP2/cC2*0.001504;
  if(jC1) jP1avg = (float)jP1/jC1*0.001504;
  if(jC2) jP2avg = (float)jP2/jC2*0.001504;
  
  vykon1 = cP1avg - jP1avg;
  vykon2 = cP2avg - jP2avg;

  cos1 = 1;
  cos2 = 1;
  if(cP1avg+jP1avg) cos1 = vykon1/(cP1avg+jP1avg);
  if(cP2avg+jP2avg) cos2 = vykon2/(cP2avg+jP2avg);
  newValues = false;
}

void setup_WDT(){
  cli();  // disable all the interrupts
  wdt_reset();
  MCUSR &= ~(1 << WDRF);    // ensure that the reset vectors are off

  // Disable and clear all Watchdog settings
  WDTCSR |= (1 << WDCE) | (1 << WDE);

  // Setup Watchdog for reset and not interrupt, and a approximately 1000ms timeout
  WDTCSR =  (1 << WDP3) | (1 << WDP0) | (0 << WDE);
  /*
  WDE | WDIE | Action
   0  |  0   |  0
   0  |  1   |  Int
   1  |  0   |  Res
   1  |  1   |  Int + Res
  */

  /*
  WDPx
  3|2|1|0|time[ms]
  0 0 0 0   16
  0 0 0 1   32
  0 0 1 0   64
  0 0 1 1  125
  0 1 0 0  250
  0 1 0 1  500
  0 1 1 0 1000
  0 1 1 1 2000
  1 0 0 0 4000
  1 0 0 1 8000
  */
  sei();  // re-enable interrupts
}
void setup_ADC(){
  ADCSRA &= ~(1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0); //clear prescalers

  // Set prescalers enable ADC and ISR(ADC_comp_vect)
  ADCSRA |= 1<<ADEN | 1<<ADIE | 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0;
  // Set multiplexer reference voltage and input pin A2
  ADMUX = (1<<REFS0) | (1<<MUX1);

  sei();
  // Start conversion
  ADCSRA |= 1<<ADSC;
}
void setup_timer(){
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 3036;
  TCCR1B |= (1<<CS12);
  TIMSK1 |= (1<<TOIE1);
}

/*int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}*/
