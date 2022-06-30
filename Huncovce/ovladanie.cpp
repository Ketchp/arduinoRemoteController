#include <Arduino.h>
#include "ovladanie.h"
#include "meranie.h"
#include "functions.h"

const uint8_t ESP8266_RSTPin = 13;
const uint8_t tg1_dopreduPin = 34;
const uint8_t tg1_dozaduPin = 35;
const uint8_t tg2_dopreduPin = 36;
const uint8_t tg2_dozaduPin = 37;
const uint8_t kompenzacia_1Pin = 38;
const uint8_t ventil_1Pin = 39;
const uint8_t ventil_2Pin = 40;
const uint8_t kompenzacia_2Pin = 41;
const uint8_t cistenieVpredPin = 42;
const uint8_t cistenieVzadPin = 43;
const uint8_t stavidlaNahorPin = 44;
const uint8_t stavidlaNadolPin = 45;
const uint8_t Led1Pin = 46;
const uint8_t Led2Pin = 47;
const uint8_t watchdogLed = A4;
const uint8_t startLed = A8;


void initOutput(){
  pinMode(cistenieVpredPin, OUTPUT);
  digitalWrite(cistenieVpredPin,HIGH);
  pinMode(cistenieVzadPin, OUTPUT);
  digitalWrite(cistenieVzadPin,HIGH);
  pinMode(stavidlaNahorPin, OUTPUT);
  digitalWrite(stavidlaNahorPin,HIGH);
  pinMode(stavidlaNadolPin, OUTPUT);
  digitalWrite(stavidlaNadolPin,HIGH);
  pinMode(tg1_dopreduPin, OUTPUT);
  digitalWrite(tg1_dopreduPin,HIGH);
  pinMode(tg1_dozaduPin, OUTPUT);
  digitalWrite(tg1_dozaduPin,HIGH);
  pinMode(tg2_dopreduPin, OUTPUT);
  digitalWrite(tg2_dopreduPin,HIGH);
  pinMode(tg2_dozaduPin, OUTPUT);
  digitalWrite(tg2_dozaduPin,HIGH);
  pinMode(ventil_1Pin, OUTPUT);
  digitalWrite(ventil_1Pin,HIGH);
  pinMode(ventil_2Pin, OUTPUT);
  digitalWrite(ventil_2Pin,HIGH);
  pinMode(kompenzacia_1Pin, OUTPUT);
  digitalWrite(kompenzacia_1Pin,HIGH);
  pinMode(kompenzacia_2Pin, OUTPUT);
  digitalWrite(kompenzacia_2Pin,HIGH);
  pinMode(ESP8266_RSTPin, OUTPUT);
  digitalWrite(ESP8266_RSTPin,HIGH);
  pinMode(Led1Pin, OUTPUT);
  digitalWrite(Led1Pin,HIGH);
  pinMode(Led2Pin, OUTPUT);
  digitalWrite(Led2Pin,HIGH);
  pinMode(watchdogLed, OUTPUT);
  pinMode(startLed, OUTPUT);
  digitalWrite(startLed,HIGH);
  
}

void ovladanie(){
  if(cistenie == 0){digitalWrite(cistenieVzadPin, LOW);digitalWrite(cistenieVpredPin, HIGH);}
  else if(cistenie == 1){digitalWrite(cistenieVzadPin, HIGH);digitalWrite(cistenieVpredPin, HIGH);}
  else if(cistenie == 2){digitalWrite(cistenieVzadPin, HIGH);digitalWrite(cistenieVpredPin, LOW);}

  if(stavidla_ovladanie == 0){digitalWrite(stavidlaNadolPin, LOW);digitalWrite(stavidlaNahorPin, HIGH);}
  else if(stavidla_ovladanie == 1){digitalWrite(stavidlaNadolPin, HIGH);digitalWrite(stavidlaNahorPin, HIGH);}
  else if(stavidla_ovladanie == 2){digitalWrite(stavidlaNadolPin, HIGH);digitalWrite(stavidlaNahorPin, LOW);}

  if(tg1 == 0){digitalWrite(tg1_dozaduPin, LOW);digitalWrite(tg1_dopreduPin, HIGH);}
  else if(tg1 == 1){digitalWrite(tg1_dozaduPin, HIGH);digitalWrite(tg1_dopreduPin, HIGH);}
  else if(tg1 == 2){digitalWrite(tg1_dozaduPin, HIGH);digitalWrite(tg1_dopreduPin, LOW);}

  if(tg2 == 0){digitalWrite(tg2_dozaduPin, LOW);digitalWrite(tg2_dopreduPin, HIGH);}
  else if(tg2 == 1){digitalWrite(tg2_dozaduPin, HIGH);digitalWrite(tg2_dopreduPin, HIGH);}
  else if(tg2 == 2){digitalWrite(tg2_dozaduPin, HIGH);digitalWrite(tg2_dopreduPin, LOW);}

  if(tg1_ventil) digitalWrite(ventil_1Pin, LOW);
  else digitalWrite(ventil_1Pin, HIGH);

  if(tg2_ventil) digitalWrite(ventil_2Pin, LOW);
  else digitalWrite(ventil_2Pin, HIGH);

  if(tg1_kompenzacia) digitalWrite(kompenzacia_1Pin, LOW);
  else digitalWrite(kompenzacia_1Pin, HIGH);

  if(tg2_kompenzacia) digitalWrite(kompenzacia_2Pin, LOW);
  else digitalWrite(kompenzacia_2Pin, HIGH);

  if(tg1Led) digitalWrite(Led1Pin, LOW);
  else digitalWrite(Led1Pin, HIGH);

  if(tg2Led) digitalWrite(Led2Pin, LOW);
  else digitalWrite(Led2Pin, HIGH);

  static float lastValueVykon1 = 0;
  static float lastValueVykon2 = 0;
  static float lastValueUcinn1 = 0;
  static float lastValueUcinn2 = 0;
  if(lastValueVykon1 != vykon1){
    lastValueVykon1 = vykon1;
    int val = vykon1*10;

    Serial3.print(F("x0.val="));
    Serial3.print(val);
    Serial3.write(0xff);
    Serial3.write(0xff);
    Serial3.write(0xff);
  }
  if(lastValueVykon2 != vykon2){
    lastValueVykon2 = vykon2;
    int val = vykon2*10;

    Serial3.print(F("x1.val="));
    Serial3.print(val);
    Serial3.write(0xff);
    Serial3.write(0xff);
    Serial3.write(0xff);
  }
  if(lastValueUcinn1 != cos1){
    lastValueUcinn1 = cos1;
    int val = cos1*1000;

    Serial3.print(F("x2.val="));
    Serial3.print(val);
    Serial3.write(0xff);
    Serial3.write(0xff);
    Serial3.write(0xff);
  }
  if(lastValueUcinn2 != cos2){
    lastValueUcinn2 = cos2;
    int val = cos2*1000;

    Serial3.print(F("x3.val="));
    Serial3.print(val);
    Serial3.write(0xff);
    Serial3.write(0xff);
    Serial3.write(0xff);
  }
  while(Serial3.available()){
    Serial.println(Serial3.read());
  }
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
