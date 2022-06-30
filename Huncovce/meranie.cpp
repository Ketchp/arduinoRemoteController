#include <Arduino.h>
#include "meranie.h"
#include "functions.h"
#include "communication.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DS3231.h"
#include <EEPROM.h>
#include "ISRs.h"



//INPUT PINS
const uint8_t reedSensor_1Pin = 2;
const uint8_t reedSensor_2Pin = 3;
const uint8_t dialkovePin = 8;
const uint8_t manualnePin = 9;
const uint8_t ceslaOtockaPin = 10;
const uint8_t ceslaPretazeniePin = 11;
const uint8_t temperaturePin = 12;
const uint8_t tg1_ManualDozaduPin = 22;
const uint8_t tg1_ManualDopreduPin = 23;
const uint8_t tg2_ManualDozaduPin = 24;
const uint8_t tg2_ManualDopreduPin = 25;
const uint8_t ventil1ManualPin = 26;
const uint8_t ventil2ManualPin = 27;
const uint8_t kompenzacia1ManualPin = 28;
const uint8_t kompenzacia2ManualPin = 29;
const uint8_t stavidlaManualHorePin = 30;
const uint8_t stavidlaManualDolePin = 31;
const uint8_t cistenieManualDopreduPin = 32;
const uint8_t cistenieManualDozaduPin = 33;
const uint8_t SietovaOchranaPin = 51;
const uint8_t StavidlaOchranaPin = 52;
const uint8_t CeslaOchranaPin = 53;
const uint8_t ampermeter1Pin = A0;
const uint8_t ampermeter2Pin = A1;
const uint8_t volatageMeterPin = A2;
const uint8_t manual_RST = A3;
const uint8_t stavidlaKoncakHorePin = A14;
const uint8_t stavidlaKoncakDolePin = A15;

void pciSetup(uint8_t pin);


long realtime;

float teplota; //degC
float teplota_clock;
bool dialkove_ovladanie;
bool manualne_ovladanie;
bool not_manualne;
uint8_t stavidla_poloha; //0dole 1stred 2hore
bool cistenie_pretazenie;
bool cistenie_otocka;
volatile bool cistenieActive = true;
bool cistenieEnabled;
bool cistenieDeblokEnabled;
bool cleaning;
bool tg1_riadenie_enabled;
bool tg2_riadenie_enabled;
bool alwaysTrue = true;
volatile bool manual_cistenie = false;
volatile bool auto_cistenie = false;
bool sietovaOchrana;
bool ceslaOchrana;
bool stavidlaOchrana;

//control variables
volatile uint8_t cistenie = 1; //0vzad 1stoj 2vpred
volatile uint8_t stavidla_ovladanie = 1; //0nadol 1stoj 2nahor
uint8_t tg1 = 1; //0vzad 1stoj 2vpred
uint8_t tg2 = 1; //0vzad 1stoj 2vpred
bool tg1_ventil;
bool tg2_ventil;
bool tg1_kompenzacia;
bool tg2_kompenzacia;
bool kompDialkove;
bool tg1_cistenie;
bool tg2_cistenie;
bool tg1_priebezne;
bool tg2_priebezne;
uint8_t tg1_phase = 0;
uint8_t tg2_phase = 0;
uint8_t tg1_request = 2;
uint8_t tg2_request = 2;

//time keeping
unsigned long cistenie_zaciatok;
unsigned long stavidlo_zaciatok;
unsigned long kW1_pod = 0;
unsigned long kW2_pod = 0;
unsigned long tg1_t;
unsigned long tg2_t;
unsigned long tg1_moveStart;
unsigned long tg2_moveStart;
unsigned long last_display;
unsigned long last_temperature = 5000;
unsigned long last_time_upt;
bool tg1Led;
bool tg2Led;


OneWire oneWireDS(temperaturePin);
DallasTemperature temp_sensor(&oneWireDS);

RTClib RTC;
DS3231 Clock;


//EEPROM values
uint16_t T_cistenie; //min
float min_kW1; //kW
float min_kW2;
long posledne_cistenie; //unix s
float cistenie_t; //s
float stavidla_ovladanie_t; //s
float perioda_cistenie_tg; //h
long tg1_last_clean; //unix s
long tg2_last_clean; //unix s
uint16_t reset_count;
uint16_t turnOnAddress;
long lastRestart;

//EEPROM addresses
const uint16_t T_cistenie_eeAdd = 0;
const uint16_t min_kW1_eeAdd = sizeof(T_cistenie);
const uint16_t min_kW2_eeAdd = min_kW1_eeAdd + sizeof(min_kW1);
const uint16_t posledne_cistenie_eeAdd = min_kW2_eeAdd + sizeof(min_kW2);
const uint16_t cistenie_t_eeAdd = posledne_cistenie_eeAdd + sizeof(posledne_cistenie);
const uint16_t stavidla_ovladanie_t_eeAdd = cistenie_t_eeAdd + sizeof(cistenie_t);
const uint16_t perioda_cistenie_tg_eeAdd = stavidla_ovladanie_t_eeAdd + sizeof(stavidla_ovladanie_t);
const uint16_t tg1_last_clean_eeAdd = perioda_cistenie_tg_eeAdd + sizeof(perioda_cistenie_tg);
const uint16_t tg2_last_clean_eeAdd = tg1_last_clean_eeAdd + sizeof(tg1_last_clean);
const uint16_t reset_count_eeAdd = tg2_last_clean_eeAdd + sizeof(tg2_last_clean);
const uint16_t turnOnAddress_eeAdd = reset_count_eeAdd + sizeof(reset_count);


void initInput(){
  pinMode(reedSensor_1Pin, INPUT_PULLUP);
  pinMode(reedSensor_2Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(reedSensor_1Pin), tg1_increment, FALLING);
  attachInterrupt(digitalPinToInterrupt(reedSensor_2Pin), tg2_increment, FALLING);
  pinMode(reedSensor_1Pin, INPUT_PULLUP);
  pinMode(reedSensor_2Pin, INPUT_PULLUP);
  
  pinMode(stavidlaKoncakHorePin, INPUT_PULLUP);
  pinMode(stavidlaKoncakDolePin, INPUT_PULLUP);
  pinMode(temperaturePin, INPUT_PULLUP);
  pinMode(dialkovePin, INPUT_PULLUP);
  pinMode(manualnePin, INPUT_PULLUP);
  pinMode(tg1_ManualDozaduPin, INPUT_PULLUP);
  pinMode(tg1_ManualDopreduPin, INPUT_PULLUP);
  pinMode(tg2_ManualDozaduPin, INPUT_PULLUP);
  pinMode(tg2_ManualDopreduPin, INPUT_PULLUP);
  pinMode(ventil1ManualPin, INPUT_PULLUP);
  pinMode(ventil2ManualPin, INPUT_PULLUP);
  pinMode(cistenieManualDopreduPin, INPUT_PULLUP);
  pinMode(cistenieManualDozaduPin, INPUT_PULLUP);
  pinMode(stavidlaManualHorePin, INPUT_PULLUP);
  pinMode(stavidlaManualDolePin, INPUT_PULLUP);
  pinMode(kompenzacia1ManualPin, INPUT_PULLUP);
  pinMode(kompenzacia2ManualPin, INPUT_PULLUP);
  pinMode(SietovaOchranaPin, INPUT_PULLUP);
  pinMode(CeslaOchranaPin, INPUT_PULLUP);
  pinMode(StavidlaOchranaPin, INPUT_PULLUP);
  pinMode(manual_RST, INPUT_PULLUP);
  pinMode(ceslaOtockaPin , INPUT_PULLUP);
  pinMode(ceslaPretazeniePin, INPUT_PULLUP);
  pciSetup(stavidlaKoncakHorePin);
  pciSetup(stavidlaKoncakDolePin);
//  pciSetup(ceslaOtockaPin);
  pciSetup(ceslaPretazeniePin);

  temp_sensor.begin();
  Wire.begin();

  //todo zrychlit start


  EEPROM.get(T_cistenie_eeAdd,T_cistenie);
  EEPROM.get(min_kW1_eeAdd,min_kW1);
  EEPROM.get(min_kW2_eeAdd,min_kW2);
  EEPROM.get(posledne_cistenie_eeAdd,posledne_cistenie);
  EEPROM.get(cistenie_t_eeAdd,cistenie_t);
  EEPROM.get(stavidla_ovladanie_t_eeAdd,stavidla_ovladanie_t);
  EEPROM.get(perioda_cistenie_tg_eeAdd, perioda_cistenie_tg);
  EEPROM.get(tg1_last_clean_eeAdd, tg1_last_clean);
  EEPROM.get(tg2_last_clean_eeAdd, tg2_last_clean);
  EEPROM.get(reset_count_eeAdd, reset_count);
  EEPROM.get(turnOnAddress_eeAdd, turnOnAddress);
  reset_count++;
  EEPROM.put(reset_count_eeAdd, reset_count);
  EEPROM.get(turnOnAddress, lastRestart);
  Serial.print(F("Reset count: "));
  Serial.println(reset_count);
  Serial.print(F("turnOnAddress: "));
  Serial.println(turnOnAddress);

  if(perioda_cistenie_tg < 0 ||  (0 < perioda_cistenie_tg && perioda_cistenie_tg < 1)){
    perioda_cistenie_tg = 0;
  }

  randomSeed(analogRead(A7));
}


//measured variables
volatile unsigned int tg1_rot = 1;
volatile unsigned int tg2_rot = 1;
const uint8_t otackyBuffSize = 10;
unsigned long last_rot = 0;
unsigned int otacky1Buff[otackyBuffSize];
unsigned int otacky2Buff[otackyBuffSize];
unsigned int otacky1Pref;
unsigned int otacky2Pref;
uint8_t otackyPointer;
int otacky_1 = 1; //RMP
int otacky_2 = 1; // porucha -> -1


void meranie(){
  DateTime now = RTC.now();
  realtime = now.unixtime();

  if((unsigned long)(millis() - last_rot) > 1000ul){
    last_rot = millis();
    otacky1Pref -= otacky1Buff[otackyPointer];
    otacky2Pref -= otacky2Buff[otackyPointer];
    noInterrupts();
    otacky1Buff[otackyPointer] = tg1_rot;
    otacky2Buff[otackyPointer] = tg2_rot;
    tg1_rot = 0;
    tg2_rot = 0;
    interrupts();
    if(otacky_1 != -1){
      if(!otacky1Buff[otackyPointer]){
        if((unsigned long)(millis() - tg1_moveStart) > 5000ul && tg1 != 1){
          otacky_1 = -1;
          if(tg1_cistenie || tg1_priebezne){
            tg1_request = 1;
            tg1_cistenie = false;
            tg1_priebezne = false;
            tg1_phase = 0;
          }
        }
        else otacky_1 = 0;
      }
      else{
        otacky1Pref += otacky1Buff[otackyPointer];
        otacky_1 = otacky1Pref*60/otackyBuffSize;
      }
    }
    if(otacky_2 != -1){
      if(!otacky2Buff[otackyPointer]){
        if((unsigned long)(millis() - tg2_moveStart) > 5000ul && tg2 != 1){
          otacky_2 = -1;
          if(tg2_cistenie || tg2_priebezne){
            tg2_request = 1;
            tg2_cistenie = false;
            tg2_priebezne = false;
            tg2_phase = 0;
          }
        }
        else otacky_2 = 0;
      }
      else{
        otacky2Pref += otacky2Buff[otackyPointer];
        otacky_2 = otacky2Pref*60/otackyBuffSize;
      }
    }

    otackyPointer = (otackyPointer + 1) % otackyBuffSize;
  }

  if((unsigned long)(millis() - last_temperature) > 300000ul){
    temp_sensor.requestTemperatures();
    teplota = temp_sensor.getTempCByIndex(0);
    last_temperature = millis();
  }/***/
  
  teplota_clock = Clock.getTemperature();
  if(newValues){
    calculatePower();
  }
  //getPower();
  dialkove_ovladanie = !digitalRead(dialkovePin);
  manualne_ovladanie = !digitalRead(manualnePin);
  not_manualne = !manualne_ovladanie;
  if(!digitalRead(stavidlaKoncakHorePin)) stavidla_poloha = 2;
  else if(!digitalRead(stavidlaKoncakDolePin)) stavidla_poloha = 0;
  else stavidla_poloha = 1;

  ceslaOchrana = digitalRead(CeslaOchranaPin) && digitalRead(ceslaPretazeniePin);
  stavidlaOchrana = digitalRead(StavidlaOchranaPin);

  sietovaOchrana = !digitalRead(SietovaOchranaPin);

  cistenieActive &= ceslaOchrana;
  cistenieEnabled = cistenieActive && dialkove_ovladanie;
  cistenieDeblokEnabled = !manualne_ovladanie && ceslaOchrana && !cistenieActive;

  if(!digitalRead(ceslaOtockaPin)){
    cistenie_otocka = true;
  }

  if(!digitalRead(manual_RST) && !need_restart){
    need_restart = true;
    wait_start = 0;
  }
  cleaning = tg1_cistenie || tg2_cistenie || tg1_priebezne || tg2_priebezne;
  tg1_riadenie_enabled = !cleaning && dialkove_ovladanie && otacky_1 != -1;
  tg2_riadenie_enabled = !cleaning && dialkove_ovladanie && otacky_2 != -1;

  if(dialkove_ovladanie){
    if(!digitalRead(cistenieManualDopreduPin)){
      cistenie = 2;
      cistenie_zaciatok = millis();
      auto_cistenie = false;
      manual_cistenie = true;
    }
    else if(!digitalRead(cistenieManualDozaduPin)){
      cistenie = 0;
      auto_cistenie = false;
      manual_cistenie = true;
    }
    else if(manual_cistenie){
      manual_cistenie = false;
      cistenie = 1;
      cistenieActive = true;
    }

    if(!cistenieActive){
      cistenie = 1;
    }
  }
  else if(manualne_ovladanie){
    tg1_cistenie = false;
    tg1_priebezne = false;
    tg1_phase = 0;
    tg2_cistenie = false;
    tg2_priebezne = false;
    tg2_phase = 0;
    if(!digitalRead(cistenieManualDopreduPin)){
      cistenie = 2;
      cistenie_zaciatok = millis();
    }
    else if(!digitalRead(cistenieManualDozaduPin)) cistenie = 0;
    else{
      cistenie = 1;
      cistenieActive = true;
      auto_cistenie = false;
    }
    if(!cistenieActive){
      cistenie = 1;
    }

    if(!digitalRead(tg1_ManualDopreduPin)) tg1_request = 2;
    else if(!digitalRead(tg1_ManualDozaduPin)) tg1_request = 0;
    else tg1_request = 1;
    if(!digitalRead(tg2_ManualDopreduPin)) tg2_request = 2;
    else if(!digitalRead(tg2_ManualDozaduPin)) tg2_request = 0;
    else tg2_request = 1;
    tg1_ventil = !digitalRead(ventil1ManualPin);
    tg2_ventil = !digitalRead(ventil2ManualPin);
    tg1_kompenzacia = !digitalRead(kompenzacia1ManualPin);
    tg2_kompenzacia = !digitalRead(kompenzacia2ManualPin);
    if(!digitalRead(stavidlaManualHorePin) && stavidla_poloha != 2) stavidla_ovladanie = 2;
    else if(!digitalRead(stavidlaManualDolePin) && stavidla_poloha != 0) stavidla_ovladanie = 0;
    else stavidla_ovladanie = 1;
  }
  else{
    if(tg1_priebezne){
      tg1_phase = 0;
      tg1_priebezne = false;
    }
    if(tg2_priebezne){
      tg2_phase = 0;
      tg2_priebezne = false;
    }
    if(!digitalRead(tg1_ManualDopreduPin)){
      tg1_request = 2;
    }
    if(!digitalRead(tg2_ManualDopreduPin)){
      tg2_request = 2;
    }
    if(!digitalRead(tg1_ManualDozaduPin)){
      tg1_request = 1;
    }
    if(!digitalRead(tg2_ManualDozaduPin)){
      tg2_request = 1;
    }

    if(!digitalRead(stavidlaManualHorePin) && stavidla_poloha != 2) stavidla_ovladanie = 2;
    else if(!digitalRead(stavidlaManualDolePin) && stavidla_poloha != 0) stavidla_ovladanie = 0;
    else stavidla_ovladanie = 1;
  }
}

void pciSetup(uint8_t pin){
  *digitalPinToPCMSK(pin) |= 1 << digitalPinToPCMSKbit(pin);  // enable pin
  PCIFR  |= 1 << digitalPinToPCICRbit(pin); // clear any outstanding interrupt
  PCICR  |= 1 << digitalPinToPCICRbit(pin); // enable interrupt for the group
}

/*
0  - RX0 - debug     INPUT
1  - TX0 - debug     OUTPUT
2  - REED1           INPUT
3  - REED2           INPUT
4  -
5  -
6  -
7  -
8  - dialkovePin      INPUT
9  - manualnePin      INPUT
10 - ceslaOtockaPin        INPUT
11 - ceslaPretazeniePin    INPUT
12 - temperaturePin        INPUT
13 - ESP-RST               OUTPUT
14 - //
15 - //
16 - TX2 - GSM        OUTPUT
17 - RX2 - GSM        INPUT
18 - TX1 - ESP        OUTPUT
19 - RX1 - ESP        INPUT
20 - SDA              OUTPUT
21 - SDL              OUTPUT
22 - tg1_ManualDozaduPin      INPUT
23 - tg1_ManualDopreduPin     INPUT
24 - tg2_ManualDozaduPin      INPUT
25 - tg2_ManualDopreduPin     INPUT
26 - ventil1ManualPin         INPUT
27 - ventil2ManualPin         INPUT
28 - kompenzacia1ManualPin    INPUT
29 - kompenzacia2ManualPin    INPUT
30 - stavidlaManualHorePin    INPUT
31 - stavidlaManualDolePin    INPUT
32 - cistenieManualDopreduPin INPUT
33 - cistenieManualDozaduPin  INPUT
34 - tg1_dopreduPin   OUTPUT
35 - tg1_dozaduPin    OUTPUT
36 - tg2_dopreduPin   OUTPUT
37 - tg2_dozaduPin    OUTPUT
38 - kompenzacia_1Pin OUTPUT
39 - ventil_1Pin      OUTPUT
40 - ventil_2Pin      OUTPUT
41 - kompenzacia_2Pin OUTPUT
42 - cistenieVpredPin OUTPUT
43 - cistenieVzadPin  OUTPUT
44 - stavidlaNahorPin OUTPUT
45 - stavidlaNadolPin OUTPUT
46 - Led1Pin          OUTPUT
47 - Led2Pin          OUTPUT
48 -
49 -
50 -
51 - SietovaOchranaPin        INPUT
52 - StavidlaOchranaPin       INPUT
53 - CeslaOchranaPin          INPUT
A0 - ampermeter1Pin           A_INPUT
A1 - ampermeter2Pin           A_INPUT
A2 - volatageMeterPin         A_INPUT
A3 - manual_RST               INPUT
A4 - watchdog_led             OUTPUT
A5 -
A6 -
A7 -
A8 - startLed                 OUTPUT
A9 -
A10-
A11-
A12-
A13-
A14- stavidlaKoncakHorePin    INPUT
A15- stavidlaKoncakDolePin    INPUT
*/
