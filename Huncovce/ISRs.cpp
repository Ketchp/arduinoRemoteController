#include <Arduino.h>
#include "ISRs.h"
#include "ovladanie.h"
#include "meranie.h"

volatile uint8_t interr1;
volatile uint8_t interr2;

ISR(PCINT0_vect){
  ++interr1;
  if(cistenie == 1) return;
  digitalWrite(cistenieVzadPin, HIGH);
  digitalWrite(cistenieVpredPin, HIGH);
  cistenieActive = false;
  cistenie = 1;
  auto_cistenie = false;
}

//void stavidlaInterrupted() //todo when released
ISR(PCINT2_vect){
  ++interr2;
  stavidla_ovladanie = 1;
  digitalWrite(stavidlaNadolPin, HIGH);
  digitalWrite(stavidlaNahorPin, HIGH);
}

volatile uint32_t cP1;
volatile uint32_t cP2;
volatile uint32_t jP1;
volatile uint32_t jP2;
volatile uint16_t cC1;
volatile uint16_t jC1;
volatile uint16_t cC2;
volatile uint16_t jC2;
volatile bool newValues;

static volatile int16_t amp1;
static volatile int16_t amp2;
static volatile int16_t volt1;
static volatile int16_t volt2;
static volatile bool measureFlop;
static volatile uint32_t cinny1;
static volatile uint32_t cinny2;
static volatile uint32_t jalovy1;
static volatile uint32_t jalovy2;
static volatile uint16_t sampCP1;
static volatile uint16_t sampJP1;
static volatile uint16_t sampCP2;
static volatile uint16_t sampJP2;

ISR(TIMER1_OVF_vect){
  cP1 = cinny1;
  cP2 = cinny2;
  jP1 = jalovy1;
  jP2 = jalovy2;
  cC1 = sampCP1;
  jC1 = sampJP1;
  cC2 = sampCP2;
  jC2 = sampJP2;
  cinny1 = 0;
  cinny2 = 0;
  jalovy1 = 0;
  jalovy2 = 0;
  sampCP1 = 0;
  sampJP1 = 0;
  sampCP2 = 0;
  sampJP2 = 0;
  newValues = true;
  TCNT1 = 3036;
}

ISR(ADC_vect){
  uint8_t theLow = ADCL;
  uint16_t result = theLow | ADCH << 8;
  sei();
  switch(ADMUX){
    case 0x40:{
      ADMUX = 0x42;
      amp1 = result-510;
      int32_t actVykon1 = (int32_t)amp1*volt1;
      if(actVykon1 >= 0){
        cinny1 += actVykon1;
        ++sampCP1;
      }
      else{
        jalovy1 += -actVykon1;
        ++sampJP1;
      }
      break;
    }
    case 0x41:{
      ADMUX = 0x42;
      amp2 = result-510;
      int32_t actVykon2 = (int32_t)amp2*volt2;
      if(actVykon2 >= 0){
        cinny2 += actVykon2;
        ++sampCP2;
      }
      else{
        jalovy2 += -actVykon2;
        ++sampJP2;
      }
      break;
    }
    case 0x42:
      if(measureFlop){
        volt2 = result-510;
        ADMUX = 0x41;
        measureFlop = false;
      }
      else{
        volt1 = result-510;
        ADMUX = 0x40;
        measureFlop = true;
      }
      break;
  }
  
  ADCSRA |= 1<<ADSC;
}
