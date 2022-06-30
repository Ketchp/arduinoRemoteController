#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include <Wire.h>
#include <MemoryFree.h>
#include "DS3231.h"
#include <stdarg.h>
#include <avr/wdt.h>

#include "functions.h"
#include "communication.h"
#include "meranie.h"
#include "logika.h"
#include "ovladanie.h"
#include "ISRs.h"

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600); //AT+UART_DEF=9600,8,1,0,0
  Serial2.begin(9600);
  Serial3.begin(9600);
  Serial.println(F("Begin"));

  initOutput();
  initInput();

  setup_WDT();
  setup_timer();
  setup_ADC();

}

void loop() {
  wdt_reset();

  meranie();
  komunikacia();
  logika();
  ovladanie();
  
  if(interr1){
    Serial.print(F("int_cesla: "));
    Serial.println(interr1);
    interr1 = 0;
  }
  if(interr2){
    Serial.print(F("int_stavidla: "));
    Serial.println(interr2);
    interr2 = 0;
  }

  static uint8_t loopCount = 0;
  if(!loopCount) digitalWrite(watchdogLed, !digitalRead(watchdogLed));
  loopCount++;

  if(digitalRead(startLed) && millis() > 15000ul){
    digitalWrite(startLed, LOW);
    if(turnOnAddress < 4000){
      turnOnAddress += sizeof(realtime);
      EEPROM.put(turnOnAddress_eeAdd, turnOnAddress);
    }
    EEPROM.put(turnOnAddress, realtime);
    Serial.println(F("Restart times:"));
    long rstTime;
    for(uint16_t EEaddress = turnOnAddress_eeAdd + sizeof(turnOnAddress); EEaddress < turnOnAddress; EEaddress+=sizeof(realtime)){
      EEPROM.get(EEaddress, rstTime);
      Serial.print(rstTime);
      Serial.print(" at:");
      Serial.println(EEaddress);
    }
    lastRestart = rstTime;
  }
  while(Serial.available()){
    char c = Serial.read();
    if(c == '~'){
      printRecBuffer();
      Serial.read();Serial.read();
    }
    else if(c == '!'){
      printRecBuffer();
      clearBuffer();
    }
    else Serial1.write(c);
  }
}
