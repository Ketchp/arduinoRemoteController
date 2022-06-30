#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <MemoryFree.h>
#include "communication.h"
#include "functions.h"
#include "meranie.h"
#include "ovladanie.h"

#define MAX_RESP_TIME 1000
#define MAX_SET_UP_TIME 3000

const PROGMEM char info[] = "{\"STATUS\":\"TEMPLATE\",\"version\":1,\"geometry\":{\"cols\":4},\"widgets\":[\n{\"N\":\"Vykon 1: \",\"W\":\"V\",\"T\":\"F\",\"U\":\"kW\"},\n{\"N\":\"Vykon 2: \",\"W\":\"V\",\"T\":\"F\",\"U\":\"kW\"},\n{\"N\":\"Posledne cistenie: \",\"W\":\"V\",\"T\":\"DT\",\"U\":\"%d.%m. / %H:%M:%S\"},\n{\"N\":\"Perioda cistenia: \",\"W\":\"E\",\"T\":\"I\",\"U\":\"min\",\"V\":{\"0\":\"Vypnute\"}},\n{\"N\":\"Min vykon1: \",\"W\":\"E\",\"T\":\"F\",\"U\":\"kW\"},\n{\"N\":\"Min vykon2: \",\"W\":\"E\",\"T\":\"F\",\"U\":\"kW\"},\n{\"N\":\"Cistenie\",\"W\":\"TS\",\"T\":\"B\",\"text\":\"Dozadu,Stoj,Dopredu\"},\n{\"N\":\"Stavidla\",\"W\":\"TS\",\"T\":\"B\",\"text\":\"Nadol,Stoj,Nahor\"},\n{\"N\":\"Tg1\",\"W\":\"TS\",\"T\":\"B\",\"text\":\"Spatny chod,Vypnuty,Normalny chod\"},\n{\"N\":\"Tg2\",\"W\":\"TS\",\"T\":\"B\",\"text\":\"Spatny chod,Vypnuty,Normalny chod\"},\n{\"N\":\"Cas spustenia: \",\"W\":\"E\",\"T\":\"F\",\"U\":\"s\"},\n{\"N\":\"Cas spustenia: \",\"W\":\"E\",\"T\":\"F\",\"U\":\"s\"},\n{\"N\":\"Priebezne otacky 1\",\"W\":\"B\",\"T\":\"b\"},\n{\"N\":\"Priebezne otacky 2\",\"W\":\"B\",\"T\":\"b\"},\n{\"N\":\"Deblok cistenia\",\"W\":\"B\",\"T\":\"b\"},\n{\"N\":\"Poloha stavidla: \",\"W\":\"T\",\"T\":\"B\",\"text\":\"Dole,Stred,Hore\"},\n{\"N\":\"Otacky 1: \",\"W\":\"V\",\"T\":\"I\",\"U\":\"RPM\",\"V\":{\"-1\":\"Porucha\"}},\n{\"N\":\"Otacky 2: \",\"W\":\"V\",\"T\":\"I\",\"U\":\"RPM\",\"V\":{\"-1\":\"Porucha\"}},\n{\"N\":\"Cesla ochrana: \",\"W\":\"EN\",\"T\":\"b\",\"text\":\"Vypnuta,Zapnuta\"},\n{\"N\":\"Stavidla ochrana: \",\"W\":\"EN\",\"T\":\"b\",\"text\":\"Vypnuta,Zapnuta\"},\n{\"N\":\"Tg1 cistenie\",\"W\":\"B\",\"T\":\"b\"},\n{\"N\":\"Tg2 cistenie\",\"W\":\"B\",\"T\":\"b\"},\n{\"N\":\"Teplota von: \",\"W\":\"V\",\"T\":\"F\",\"U\":\"<deg>C\"},\n{\"N\":\"Teplota dnu: \",\"W\":\"V\",\"T\":\"F\",\"U\":\"<deg>C\"},\n{\"N\":\"Posledne cistenie tg1: \",\"W\":\"V\",\"T\":\"DT\",\"U\":\"%d.%m. / %H:%M:%S\"},\n{\"N\":\"Posledne cistenie tg2: \",\"W\":\"V\",\"T\":\"DT\",\"U\":\"%d.%m. / %H:%M:%S\"},\n{\"N\":\"Sietova ochrana: \",\"W\":\"EN\",\"T\":\"b\",\"text\":\"Vypnuta,Zapnuta\"},\n{\"N\":\"Perioda cistenia tg: \",\"W\":\"E\",\"T\":\"F\",\"U\":\"h\",\"V\":{\"0\":\"Vypnute\"}},\n{\"N\":\"TG1 ucinnik: \",\"W\":\"V\",\"T\":\"F\"},\n{\"N\":\"TG2 ucinnik: \",\"W\":\"V\",\"T\":\"F\"},\n{\"N\":\"Tg1 kompenzacia\",\"W\":\"S\",\"T\":\"b\"},\n{\"N\":\"Tg2 kompenzacia\",\"W\":\"S\",\"T\":\"b\"}]}";
const PROGMEM char login[] = "{\"STATUS\": \"TEMPLATE\",\"version\":1,\"geometry\":{\"cols\":1},\n\"widgets\":[{\"N\":\"Heslo:\",\"W\":\"entry\",\"T\":\"str\",\"show\":\"password\"}],\n\"log\":{\"key\":#}}";
const PROGMEM char notauthorised[] = "{\"STATUS\":\"TEMPLATE\",\"version\":1,\"geometry\":{\"cols\":1},\"widgets\":[\n{\"N\":\"Nespravne heslo\",\"W\":\"label\"},\n{\"N\":\"Heslo:\",\"W\":\"entry\",\"T\":\"str\",\"show\":\"password\"}]}";
const PROGMEM char update[] = "{\"STATUS\":\"UPDATE\",\"version\":1,\"values\":[#,#,#,[#,#],[#,#],[#,#],[#,#],[#,#],[#,#],[#,#],[#,#],[#,#],[#,#],[#,#],[#,#],#,#,#,#,#,[#,#],[#,#],#,#,#,#,#,[#,#],#,#,[#,#],[#,#]],\"log\":{\"count\":#,\"last\":#}}";

const char PASSWORD[] = "6969";

/*
const char ssid[] = "Kornaj-Tenda";
const char pswd[] = "9977553311";
const int ESP8266_IP[4] = {192,168,2,199};
const int port = 9874;
*/

/*
const char ssid[] = "  Ketchp";
const char pswd[] = "abcdefgh";
const int ESP8266_IP[4] = {192,168,43,199};
const int port = 9874;
*/

const char ssid[] = "Huncovce";
const char pswd[] = "tmp358609";
const int ESP8266_IP[4] = {192,168,0,199};
const int port = 9874;

/*
const char ssid[] = "ASUS_68";
const char pswd[] = "klara 98";
const int ESP8266_IP[4] = {192,168,50,199};
const int port = 9874;
*/

enum is_type{ is_byte, is_bool, is_long, is_int, is_float};

union types{
  uint8_t byte_val;
  bool bool_val;
  long long_val;
  int int_val;
  float float_val;
};

struct all_type{
  enum is_type type;
  volatile void *ptr;
};


struct all_type update_var[] = {
                        {is_float,&vykon1},//2
                        {is_float,&vykon2},//3
                        {is_long,&posledne_cistenie},//6
                        {is_int,&T_cistenie},{is_bool,&alwaysTrue},//19
                        {is_float,&min_kW1},{is_bool,&dialkove_ovladanie},//20
                        {is_float,&min_kW2},{is_bool,&dialkove_ovladanie},//21
                        {is_byte,&cistenie},{is_bool,&cistenieEnabled},//11
                        {is_byte,&stavidla_ovladanie},{is_bool,&dialkove_ovladanie},//13
                        {is_byte,&tg1},{is_bool,&tg1_riadenie_enabled},//15
                        {is_byte,&tg2},{is_bool,&tg2_riadenie_enabled},//16
                        {is_float,&cistenie_t},{is_bool,&dialkove_ovladanie},//12
                        {is_float,&stavidla_ovladanie_t},{is_bool,&dialkove_ovladanie},//14
                        {is_bool,&tg1_priebezne},{is_bool,&tg1_riadenie_enabled},//17
                        {is_bool,&tg2_priebezne},{is_bool,&tg2_riadenie_enabled},//18
                        {is_bool,&cistenieActive},{is_bool,&cistenieDeblokEnabled},//8
                        {is_byte,&stavidla_poloha},//7
                        {is_int,&otacky_1},//0
                        {is_int,&otacky_2},//1
                        {is_bool,&ceslaOchrana},//23
                        {is_bool,&stavidlaOchrana},//24
                        {is_bool,&tg1_cistenie},{is_bool,&tg1_riadenie_enabled},//9
                        {is_bool,&tg2_cistenie},{is_bool,&tg2_riadenie_enabled},//10
                        {is_float,&teplota},//4
                        {is_float,&teplota_clock},//5
                        {is_long,&tg1_last_clean},//26
                        {is_long,&tg2_last_clean},//27
                        {is_bool,&sietovaOchrana},//22
                        {is_float,&perioda_cistenie_tg},{is_bool,&alwaysTrue},
                        {is_float,&cos1},
                        {is_float,&cos2},
                        {is_bool,&tg1_kompenzacia},{is_bool,&tg1_riadenie_enabled},//9
                        {is_bool,&tg2_kompenzacia},{is_bool,&tg2_riadenie_enabled},//10                        
                        {is_int, &reset_count},
                        {is_long, &lastRestart}
                        /**/};
                        
const uint8_t update_var_size = sizeof(update_var)/sizeof(update_var[0]);
union types copy[update_var_size];


const uint8_t sendBufferSize = 16;
const uint8_t recBufferSize = 128;
char receptionBuffer[recBufferSize];
char send_buff[sendBufferSize];
uint8_t receptionStart = 0;
uint8_t receptionEnd = 0;

void receiveESP(){
  while(Serial1.available()){
    receptionBuffer[receptionEnd] = Serial1.read();
    receptionEnd = (receptionEnd + 1) % recBufferSize;
  }
}
void clearBuffer(){
  receptionStart = 0;
  receptionEnd = 0;
}
bool search(const char* tmp){
  int tmp_len = strlen(tmp);
  for(int i = 0; i < recBufferSize; i++){
    bool wrong = false;
    for(int shift = 0; shift < tmp_len; shift++){
      int index = (receptionStart+i+shift)%recBufferSize;
      if(receptionBuffer[index] != tmp[shift]){
        wrong = true;
        break;
      }
    }
    if(!wrong){
      for(int shift = 0; shift < tmp_len; shift++){
        int index = (receptionStart+i+shift)%recBufferSize;
        receptionBuffer[index] = 'X';
      }
      receptionStart += i + tmp_len;
      receptionEnd = receptionStart;
      return true;
    }
  }
  return false;
}
//new parser
/*
bool parse(const char* tmp, ...){
  Serial.print("in");
  va_list args;
  va_start(args, tmp);
  uint8_t size = 0;
  for(uint8_t i = 0; tmp[i] != '\0'; ++i){
    if(tmp[i] == '%' && tmp[i+1] == 'd') ++size;
  }
  uint8_t targets[size];
  uint8_t target = 0;
  uint8_t a = 0;
  uint8_t b = 0;
  while(tmp[a] != '\0'){
    if(tmp[a] == '%'){
      a++;
      if(tmp[a] == 'd'){
        ++a;
        targets[target] = b;
        ++target;
        while((receptionStart+b)%recBufferSize != receptionEnd){
          if(48 <= receptionBuffer[(receptionStart+b)%recBufferSize] && receptionBuffer[(receptionStart+b)%recBufferSize] <= 57){
            ++b;
          }
          else break;
        }
      }
    }
    else if(tmp[a] != receptionBuffer[(receptionStart+b)%recBufferSize] || (receptionStart+b)%recBufferSize == receptionEnd){
      va_end(args);
      Serial.println("out");
        Serial.flush();
      return false;
    }
    else{
      ++a;
      ++b;
    }
  }
  uint8_t tar = 0;
  for(uint8_t i = 0; tmp[i] != '\0'; ++i){
    if(tmp[i] == '%' && tmp[i+1] == 'd'){
      char buffer[16];
      uint8_t start = targets[tar];
      ++tar;
      uint8_t a = 0;
      while(48 <= receptionBuffer[(receptionStart+start)%recBufferSize] && receptionBuffer[(receptionStart+start)%recBufferSize] <= 57){
        buffer[a] = receptionBuffer[(receptionStart+start)%recBufferSize];
        ++a;
        ++start;
      }
      buffer[start] = '\0';
      int _;
      if(sscanf(buffer, "%d", &_) == EOF){
        va_end(args);
        Serial.println("out");
        Serial.flush();
        return false;
      }
    }
  }
  tar = 0;
  for(uint8_t i = 0; tmp[i] != '\0'; ++i){
    if(tmp[i] == '%' && tmp[i+1] == 'd'){
      char buffer[16];
      uint8_t start = targets[tar];
      ++tar;
      uint8_t a = 0;
      while(48 <= receptionBuffer[(receptionStart+start)%recBufferSize] && receptionBuffer[(receptionStart+start)%recBufferSize] <= 57){
        buffer[a] = receptionBuffer[(receptionStart+start)%recBufferSize];
        ++a;
        ++start;
      }
      buffer[start] = '\0';
      sscanf(buffer, "%d", va_arg(args, int*));
    }
  }

  receptionStart = (receptionStart+b)%recBufferSize;
  va_end(args);
  Serial.println("out");
        Serial.flush();
  return true;
}*/

bool parse_P(PGM_P tmp, ...){
  va_list args;
  va_start(args, tmp);
  uint8_t a = 0;
  uint8_t b = 0;
  while(pgm_read_byte(tmp+a) != '\0'){
    if(pgm_read_byte(tmp+a) == '%'){
      a++;
      if(pgm_read_byte(tmp+a) == 'd'){
        a++;
        char temp[8];
        uint8_t i = 0;
        while((receptionStart+b)%recBufferSize != receptionEnd){
          if(48 <= receptionBuffer[(receptionStart+b)%recBufferSize] && receptionBuffer[(receptionStart+b)%recBufferSize] <= 57){
            temp[i] = receptionBuffer[(receptionStart+b)%recBufferSize];
            i++;
            b++;
          }
          else break;
        }
        temp[i] = '\0';
        if(sscanf(temp, "%d", va_arg(args, int*)) == EOF){
          return false;
        }
      }
    }
    else if(pgm_read_byte(tmp+a) != receptionBuffer[(receptionStart+b)%recBufferSize] || (receptionStart+b)%recBufferSize == receptionEnd){
      return false;
    }
    else{
      a++;
      b++;
    }
  }
  va_end(args);
  receptionStart = (receptionStart+b)%recBufferSize;
  return true;
}


//todo json buffer owerflow and all variables clear
int recv(char* input_buffer,int n, int buff_size){
  int start = 0;
  int shift = 0;
  while(input_buffer[start] != '\0') start++;
  while(receptionStart != receptionEnd && shift < n){
    if(start + shift < buff_size) input_buffer[start+shift] = receptionBuffer[receptionStart];
    shift++;
    receptionStart = (receptionStart+1)%recBufferSize;
  }
  if(start + shift < buff_size) input_buffer[start+shift] = '\0';
  else input_buffer[buff_size-1] = '\0';
  return n - shift;
}

int sendBuffBytes(int burst_size){
  int buff_size = strlen(send_buff);
  int sent = min(burst_size, buff_size);
  Serial1.write(send_buff, sent);
  int start = 0;
  while(sent + start <= buff_size){
    send_buff[start] = send_buff[sent + start];
    start++;
  }
  return sent;
}

void printRecBuffer(){
  Serial.println(F("________"));
  for(int i = 0; i < recBufferSize; i++){
    int index = (receptionStart+i) % recBufferSize;
    if(index == receptionEnd) Serial.print('|');
    char character = receptionBuffer[index];
    switch(character){
      case ' ':
        Serial.print("\\32");
        break;
      case '\r':
        Serial.print("\\r");
        break;
      case '\n':
        Serial.println("\\n");
        break;
      default:
        Serial.write(character);
    }
  }
  Serial.println(F("^^^^^^^^"));
}

const uint8_t clientMax = 5;
int8_t connections[clientMax] = {-1,-1,-1,-1,-1};
long connectionKey[clientMax] = {-1,-1,-1,-1,-1};
uint32_t connectionActivity[clientMax] = {0,0,0,0,0};
int8_t recClient;

const uint8_t keyMemSize = 5;
long keys[keyMemSize];
int8_t state[keyMemSize];
uint8_t lastKey = 0;

bool newReady;
bool receiving;
unsigned long wait_start;
uint8_t stage;
int CWMODE_value;
bool IP;

const uint8_t jsonBufferSize = 128;
char json[clientMax][jsonBufferSize];
uint16_t json_leng;
char response[64];
int sendClient = -1;
int8_t sendStage;

int send_length;
int packet;
int sent;
const char* ptr;
int fileShift;
int update_var_curr;
bool text_response;
unsigned long last_activity;
unsigned long rst_start;

bool need_restart;

void komunikacia(){
  receiveESP();
  while(parse_P( (const char *)F("\r") ) || parse_P((const char *)F("\n")));
  if(search("ready") || newReady){
    stage = 1;
    for(int c = 0; c < clientMax; c++){
      connections[c] = -1;
      json[c][0] = '\0';
    }
    receiving = false;
    recClient = -1;
    sendStage = 0;
    CWMODE_value = 0;
    IP = false;
    need_restart = false;
    rst_start = 0;
    wait_start = 0;
    Serial.println(F("ESP READY"));
    last_activity = millis();
    newReady = false;
  }
  
  if(need_restart){
    if(wait_start){
      if((unsigned long)(millis() - wait_start) > MAX_RESP_TIME){
        if(rst_start){
          if((unsigned long)(millis() - rst_start) > 500){
            digitalWrite(ESP8266_RSTPin, HIGH);
            rst_start = 0;
            need_restart = false;
            Serial.println(F("power on"));
          }
        }
        else{
          digitalWrite(ESP8266_RSTPin, LOW);
          rst_start = millis() | 1;
          Serial.println(F("hard rest"));
        }
      }
    }
    else{
      Serial.println(F("REST"));
      Serial1.println(F("AT+RST"));
      wait_start = millis() | 1;
    }
  }
  else if(stage == 1){
    if(wait_start){
      if(parse_P((const char *)F("AT\r\r\n\r\nOK\r\n")) || parse_P((const char *)F("OK\r\n"))){
        stage++;
        wait_start = 0;
        last_activity = millis();
        Serial.println(F("AT OK"));
      }
      else if((unsigned long)(millis() - wait_start) > MAX_RESP_TIME ||
       parse_P((const char *)F("AT\r\r\n\r\nERROR\r\n")) || parse_P((const char *)F("ERROR\r\n"))){
        wait_start = 0;
        need_restart = true;
        Serial.println(F("AT failed"));
        printRecBuffer();
        clearBuffer();
      }
    }
    else{
      Serial1.println(F("AT"));
      wait_start = millis() | 1;
    }
  }
  else if(stage == 2){
    if(wait_start){
      if(parse_P((const char *)F("ATE0\r\r\n\r\nOK\r\n")) || parse_P((const char *)F("OK\r\n"))){
        stage++;
        wait_start = 0;
        last_activity = millis();
        Serial.println(F("ATE OK"));
      }
      else if((unsigned long)(millis() - wait_start) > MAX_RESP_TIME ||
       parse_P((const char *)F("ATE0\r\r\n\r\nERROR\r\n")) || parse_P((const char *)F("ERROR\r\n"))){
        wait_start = 0;
        need_restart = true;
        Serial.println(F("ATE0 failed"));
        printRecBuffer();
        clearBuffer();
      }
    }
    else{
      Serial1.println(F("ATE0"));
      wait_start = millis() | 1;
    }
  }
  else if(stage == 3){
    if(!CWMODE_value){
      if(wait_start){
        int CWval;
        if(parse_P((const char *)F("+CWMODE:%d\r\n\r\nOK\r\n"), &CWval)){
          CWMODE_value = CWval;
          if(CWMODE_value == 1){
            stage++;
          }
          wait_start = 0;
          Serial.print(F("CWMODE ans: "));
          Serial.println(CWMODE_value);
          last_activity = millis();
        }
        else if((unsigned long)(millis() - wait_start) > MAX_RESP_TIME){
          wait_start = 0;
          need_restart = 0;
          Serial.println(F("CWMODE timeout"));
          printRecBuffer();
          clearBuffer();
        }
      }
      else{
        Serial1.println(F("AT+CWMODE?"));
        wait_start = millis() | 1;
      }
    }
    else{
      if(wait_start){
        if(parse_P((const char *)F("OK\r\n"))){
          CWMODE_value = 0;
          wait_start = 0;
          Serial.println(F("CWMODE set"));
          last_activity = millis();
        }
        else if((unsigned long)(millis() - wait_start) > MAX_RESP_TIME){
          wait_start = 0;
          need_restart = true;
          Serial.println(F("CWMODE failed"));
          printRecBuffer();
          clearBuffer();
        }
      }
      else{
        Serial1.println(F("AT+CWMODE=1"));
        wait_start = millis() | 1;
      }
    }
  }
  else if(stage == 4){
    if(wait_start){
      int err_n;
      if(parse_P((const char *)F("OK\r\n"))){
        stage++;
        wait_start = 0;
        Serial.println(F("Connected to wifi"));
        last_activity = millis();
      }
      else if(parse_P((const char *)F("+CWJAP:%d\r\n\r\nFAIL\r\n"), &err_n)){
        wait_start = 0;
        Serial.print(F("Connection failed: "));
        Serial.println(err_n);
        //todo
      }
    }
    else{
      Serial1.print(F("AT+CWJAP=\""));
      Serial1.print(ssid);
      Serial1.print(F("\",\""));
      Serial1.print(pswd);
      Serial1.println(F("\""));
      wait_start = millis() | 1;
    }
  }
  else if(stage == 5){
    if(!IP){
      if(wait_start){
        int _[1];
        int tempIP[4];
        if(parse_P((const char *)F("+CIPSTA:ip:\"%d.%d.%d.%d\"\r\n+CIPSTA:gateway:\"%d.%d.%d.%d\"\r\n+CIPSTA:netmask:\"%d.%d.%d.%d\"\r\n\r\nOK\r\n"), &tempIP[0], &tempIP[1], &tempIP[2], &tempIP[3], &_[0], &_[0], &_[0], &_[0], &_[0], &_[0], &_[0], &_[0])){
          wait_start = 0;
          IP = true;
          if(tempIP[0] == ESP8266_IP[0] && tempIP[1] == ESP8266_IP[1] &&
            tempIP[2] == ESP8266_IP[2] && tempIP[3] == ESP8266_IP[3]){
            stage++;
            Serial.println(F("ESP got correct IP"));
          }
          else Serial.println(F("ESP got wrong IP"));
          last_activity = millis();
        }
        else if((unsigned long)(millis() - wait_start) > MAX_RESP_TIME){
          wait_start = 0;
          need_restart = true;
          Serial.println(F("Couldn't query IP adress"));
          printRecBuffer();
          clearBuffer();
        }
      }
      else{
        wait_start = millis() | 1;
        Serial1.println(F("AT+CIPSTA?"));
      }
    }
    else{
      if(wait_start){
        if(parse_P((const char *)F("OK\r\n"))){
          IP = false;
          wait_start = 0;
          Serial.println(F("Changed IP adress"));
          last_activity = millis();
        }
        else if((unsigned long)(millis() - wait_start) > MAX_SET_UP_TIME){
          wait_start = 0;
          need_restart = true;
          Serial.println(F("Couldn't change IP adress"));
          printRecBuffer();
          clearBuffer();
        }

      }
      else{
        Serial1.print(F("AT+CIPSTA=\""));
        Serial1.print(ESP8266_IP[0]);
        Serial1.print('.');
        Serial1.print(ESP8266_IP[1]);
        Serial1.print('.');
        Serial1.print(ESP8266_IP[2]);
        Serial1.print('.');
        Serial1.print(ESP8266_IP[3]);
        Serial1.println("\"");
        wait_start = millis() | 1;
      }
    }
  }
  else if(stage == 6){
    if(wait_start){
      if(parse_P((const char *)F("OK\r\n"))){
        stage++;
        wait_start = 0;
        Serial.println(F("CIPMUX set"));
        last_activity = millis();
      }
      else if((unsigned long)(millis() - wait_start) > MAX_RESP_TIME){
        wait_start = 0;
        need_restart = true;
        Serial.println(F("Couldn't set CIPMUX"));
        printRecBuffer();
        clearBuffer();
      }
    }
    else{
      Serial1.println(F("AT+CIPMUX=1"));
      wait_start = millis() | 1;
    }     
  }
  else if(stage == 7){
    if(wait_start){
      if(parse_P((const char *)F("OK\r\n"))){
        stage++;
        wait_start = 0;
        Serial.println(F("Server started"));
        last_activity = millis();
      }
      else if(parse_P((const char *)F("no change\r\n\r\nOK\r\n"))){
        stage++;
        wait_start = 0;
        Serial.println(F("Server already up"));
        last_activity = millis();
      }
      else if((unsigned long)(millis() - wait_start) > MAX_RESP_TIME){
        wait_start = 0;
        need_restart = true;
        Serial.println(F("Couldn't establish server"));
        printRecBuffer();
        clearBuffer();
      }
    }
    else{
      Serial1.print(F("AT+CIPSERVER=1,"));
      Serial1.println(port);
      wait_start = millis() | 1;
    }
  }
  //sending
  else if(stage == 8){
    if(sendStage == 0){
      int tempClient;
      tempClient = sendClient;
      sendClient = -1;
      for(int i = 0; i < clientMax; i++){
        int nextClient;
        nextClient = (tempClient + i) % clientMax;
        if(json[nextClient][0] != '\0' && nextClient != recClient){
          sendClient = nextClient;
          break;
        }
      }
      if(sendClient != -1){
        sendStage = 1;
      }
    }
    if(sendStage == 1){
      char instr[8];
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, json[sendClient]);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        Serial.println(json[sendClient]);
        printRecBuffer();
        strcpy_P(response, (const char *) F("{\"STATUS\":\"ERROR\",\"ERROR\":\"MESSAGE HASNT PARSED\"}"));
      }
      else{
        if(doc.containsKey("log")){
          long msg_id = (long)doc["log"]["id"];
          if(msg_id){
            Serial.print(msg_id);
          }
          long recv_key = (long)doc["log"]["key"];
          if(recv_key){
            for(int i = 0; i < keyMemSize; i++){
              if(recv_key == keys[i]){
                connections[sendClient] = state[i];
                connectionKey[sendClient] = recv_key;
              }
            }
          }
        }

        if(doc.containsKey("CMD")){
          strcpy(instr, (const char*)doc["CMD"]);
          Serial.print(instr);
          strcpy_P(response, (const char *) F("{\"STATUS\":\"ERROR\",\"ERROR\":\"CMD NOT SUPPORTED\"}"));
        }
        else{
          strcpy_P(response, (const char *) F("{\"STATUS\":\"ERROR\",\"ERROR\":\"NO CMD PROPERTY\"}"));
          strcpy_P(instr, (const char *) F("NONE"));
        }
        if(!strncmp_P(instr, (const char *) F("QUIT"), 4)){
          json[sendClient][0] = '\0';
          sendStage = 0;
          return;
        }
        if(!strncmp_P(instr, (const char *) F("SET"), 3)){
          if(doc.containsKey("ID")){
            if(doc.containsKey("value")){
              int id = doc["ID"];
              strcpy(instr, (const char *) F("UPDT"));
              if(connections[sendClient] < 2){
                if(!strcmp(PASSWORD, (const char*)doc["value"])){
                  connections[sendClient] = 2;
                  for(int i = 0; i < keyMemSize; i++){
                    if(connectionKey[sendClient] == keys[i]){
                      state[i] = 2;
                    }
                  }
                  strcpy_P(instr, (const char *) F("TEMP"));
                }
                else if(connections[sendClient] == 0){
                  connections[sendClient] = 1;
                  for(int i = 0; i < keyMemSize; i++){
                    if(connectionKey[sendClient] == keys[i]){
                      state[i] = 1;
                    }
                  }
                  strcpy_P(instr, (const char *) F("TEMP"));
                }
              }
              else{
                switch(id){
                  case 3:
                    T_cistenie = doc["value"];
                    EEPROM.put(T_cistenie_eeAdd,T_cistenie);
                    break;
                  case 4:
                    if(dialkove_ovladanie){
                      min_kW1 = /*(double)*/doc["value"];
                      EEPROM.put(min_kW1_eeAdd,min_kW1);
                    }
                    break;
                  case 5:
                    if(dialkove_ovladanie){
                      min_kW2 = /*(double)*/doc["value"];
                      EEPROM.put(min_kW2_eeAdd,min_kW2);
                    }
                    break;
                  case 6:
                    if(cistenieEnabled){
                      cistenie = doc["value"];
                      if(cistenie == 1){
                        cistenie_otocka = true;
                      }
                      auto_cistenie = false;
                      cistenie_zaciatok = millis();
                    }
                    break;
                  case 7:
                    if(dialkove_ovladanie){
                      stavidla_ovladanie = doc["value"];
                      stavidlo_zaciatok = millis();
                    }
                    break;
                  case 8:
                    if(tg1_riadenie_enabled) tg1_request = doc["value"];
                    break;
                  case 9:
                    if(tg2_riadenie_enabled) tg2_request = doc["value"];
                    break;
                  case 10:
                    if(dialkove_ovladanie){
                      cistenie_t = /*(double)*/doc["value"];
                      EEPROM.put(cistenie_t_eeAdd, cistenie_t);
                    }
                    break;
                  case 11:
                    if(dialkove_ovladanie){
                      stavidla_ovladanie_t = /*(double)*/doc["value"];
                      EEPROM.put(stavidla_ovladanie_t_eeAdd, stavidla_ovladanie_t);
                    }
                    break;
                  case 12:
                    if(tg1_riadenie_enabled) tg1_priebezne = doc["value"];
                    break;
                  case 13:
                    if(tg2_riadenie_enabled) tg2_priebezne = doc["value"];
                    break;
                  case 14:
                    if(cistenieDeblokEnabled){
                      bool Deblok = doc["value"];
                      if(Deblok){
                        cistenieActive = true;
                      }
                    }
                    break;
                  case 20:
                    if(tg1_riadenie_enabled) tg1_cistenie = doc["value"];
                    break;
                  case 21:
                    if(tg2_riadenie_enabled) tg2_cistenie = doc["value"];
                    break;
                  case 27:
                    perioda_cistenie_tg = doc["value"];
                    EEPROM.put(perioda_cistenie_tg_eeAdd, perioda_cistenie_tg);
                    break;
                  case 30:
                    if(tg1_riadenie_enabled){
                      tg1_kompenzacia = doc["value"];
                      kompDialkove = true;
                    }
                    break;
                  case 31:
                    if(tg2_riadenie_enabled){
                      tg2_kompenzacia = doc["value"];
                      kompDialkove = true;
                    }
                    break;
                  default:
                    Serial.println(F("unexpected id"));
                    strcpy_P(response, (const char *) F("{\"STATUS\":\"ERROR\",\"ERROR\":\"WRONG ID\"}"));
                    strcpy_P(instr, (const char *) F("NONE"));
                }
              }
            }
            else strcpy_P(response, (const char *) F("{\"STATUS\":\"ERROR\",\"ERROR\":\"NO value PROPERTY\"}"));
          }
          else strcpy_P(response, (const char *) F("{\"STATUS\":\"ERROR\",\"ERROR\":\"NO ID PROPERTY\"}"));
        }

        if(!strncmp_P(instr, (const char *) F("UPDT"), 4)){
          if(connections[sendClient] == 0) strcpy_P(response, (const char *) F("{\"STATUS\":\"UPDATE\"}"));
          if(connections[sendClient] == 1) strcpy_P(response, (const char *) F("{\"STATUS\":\"UPDATE\"}"));
          if(connections[sendClient] == 2) strcpy_P(response, (const char *) F("update"));
        }
        if(!strcmp(instr, "TEMP")){
          if(connections[sendClient] == 0) strcpy_P(response, (const char *) F("login"));
          if(connections[sendClient] == 1) strcpy_P(response, (const char *) F("notauthorised"));
          if(connections[sendClient] == 2) strcpy_P(response, (const char *) F("info"));
        }
      }
      json[sendClient][0] = '\0';
      text_response = !strncmp_P(response, (const char *) F("info"),4) || !strncmp_P(response, (const char *) F("login"), 5) || 
      !strncmp_P(response, (const char *) F("notauthorised"), 13) || !strncmp_P(response, (const char *) F("update"), 6);
      if(text_response){
        if(!strncmp_P(response, (const char *) F("info"), 4)) ptr = info;
        if(!strncmp_P(response, (const char *) F("login"), 5)) ptr = login;
        if(!strncmp_P(response, (const char *) F("notauthorised"), 13)) ptr = notauthorised;
        if(!strncmp_P(response, (const char *) F("update"), 6)) ptr = update;

        send_length = strlen_P(ptr);
        if(!strncmp_P(response, (const char *) F("login"), 5)){
          char buff[16];
          send_length -= 1;
          if(connectionKey[sendClient] == -1){
            long newKey = random(1,999999999ul);
            Serial.print(F("Creating new key:"));
            Serial.println(newKey);
            connectionKey[sendClient] = newKey;
            keys[lastKey] = newKey;
            lastKey = (lastKey + 1)%keyMemSize;
          }
          send_length += sprintf(buff, "%ld", connectionKey[sendClient]);

        }
        else if(!strncmp_P(response, (const char *) F("update"),6)){
          char buff[16];
          send_length -= update_var_size;
          for(int i = 0; i < update_var_size; i++){
            switch(update_var[i].type){
              case is_byte:
                send_length += sprintf(buff, "%u", *(uint8_t*)update_var[i].ptr);
                copy[i].byte_val = *(uint8_t*)update_var[i].ptr;
                break;
              case is_bool:
                send_length += *(bool*)update_var[i].ptr ? 4 : 5;
                copy[i].bool_val = *(bool*)update_var[i].ptr;
                break;
              case is_long:
                send_length += sprintf(buff, "%ld", *(long*)update_var[i].ptr);
                copy[i].long_val = *(long*)update_var[i].ptr;
                break;
              case is_int:
                send_length += sprintf(buff, "%d", *(int*)update_var[i].ptr);
                copy[i].int_val = *(int*)update_var[i].ptr;
                break;
              case is_float:
                char temporary[16];
                dtostrf(*(float*)update_var[i].ptr, 10, 3, temporary);
                send_length += sprintf(buff, "%s", temporary);
                copy[i].float_val = *(float*)update_var[i].ptr;
                break;                    
            }
          }
        }
      }
      else send_length = strlen(response);

      send_length += sprintf(send_buff, "%d:", send_length);

      sent = 0;
      update_var_curr = 0;
      fileShift = 0;
      packet = 0;
      Serial.print(F("RC>"));
      sendStage++;
    }
    if(sendStage == 2){
      if(packet){
        if(parse_P((const char *)F("OK\r\n> "))){
          Serial.print(F("BS>"));
          sendStage++;
        }
        else if(parse_P((const char *)F("ERROR\r\n"))){
          Serial.print(F("ERROR"));
          need_restart = true;
        }
        else if(parse_P((const char *)F("link is not valid\r\n\r\nERROR"))){
          Serial.print(F("wrong link"));
          sendStage = 0;
          need_restart = true;
        }
      }
      else{
        packet = min(send_length - sent, 2048);
        Serial1.print(F("AT+CIPSEND="));
        Serial1.print(sendClient);
        Serial1.print(',');
        Serial1.println(packet);
        Serial1.print(F("SEND="));
        Serial1.print(sendClient);
        Serial1.print(',');
        Serial1.println(packet);
      }
    }
    if(sendStage == 3){
      if(packet){
        Serial.print(F("sg")); //sending
        int burst = min(packet, Serial1.availableForWrite());
        packet -= burst;
        int bursted = sendBuffBytes(burst);
        if(text_response){
          sent += burst;
          burst -= bursted;
          char character;
          while(burst){
            if(strlen(send_buff)){
              burst -= sendBuffBytes(burst);
            }
            else{
              character = pgm_read_byte_near(ptr + fileShift);
              fileShift++;
              if(character == '#'){
                if(!strncmp_P(response, (const char *) F("update"), 6)){
                  switch(update_var[update_var_curr].type){
                    case is_byte:
                      sprintf(send_buff, "%u", copy[update_var_curr].byte_val);
                      break;
                    case is_bool:
                      strcpy(send_buff, copy[update_var_curr].bool_val ? "true":"false");
                      break;
                    case is_long:
                      sprintf(send_buff, "%ld", copy[update_var_curr].long_val);
                      break;
                    case is_int:
                      sprintf(send_buff, "%d", copy[update_var_curr].int_val);
                      break;
                    case is_float:
                      char temporary[16];
                      dtostrf(copy[update_var_curr].float_val, 10, 3, temporary);
                      sprintf(send_buff, "%s", temporary);
                      break;
                  }
                  update_var_curr++;
                }
                else if(!strncmp_P(response, (const char *)F("login"), 5)){
                  sprintf(send_buff, "%ld", connectionKey[sendClient]);
                }
              }
              else{
                Serial1.write(character);
                burst--;
              }
            }
          }
        }
        else{
          burst -= bursted;
          Serial1.write(response + sent, burst);
          sent += burst + bursted;
        }
      }
      else{
        int receivedBytes;
        if(parse_P((const char *)F("Recv %d bytes"), &receivedBytes)){
          Serial.print(F("RB:"));
          Serial.print(receivedBytes);
          Serial.print('>');
          last_activity = millis();
        }
        if(parse_P((const char *)F("SEND OK\r\n"))){
          Serial.print(F("send ok"));
          if(sent < send_length) sendStage = 2;
          else sendStage = 0;
        }
        else if(parse_P((const char *)F("SEND FAIL\r\n"))){
          Serial.println(F("send fail"));
          printRecBuffer();
          need_restart = true;
        }
        else if(parse_P((const char *)F("ERROR\r\n"))){
          Serial.println(F("send error"));
          printRecBuffer();
          need_restart = true;
        }
      }
    }
  }
  

  if((unsigned long)(millis() - last_activity) > 60000l){
    if(wait_start){
      if(parse_P((const char *)F("AT\r\r\n\r\nOK\r\n")) || parse_P((const char *)F("OK\r\n"))){
        wait_start = 0;
        last_activity = millis();
        Serial.println(F("OK"));
        if(stage == 0){
          newReady = true;
        }
        bool clients = false;
        for(int c = 0; c < clientMax; c++){
          if(connections[c] != -1 && (connectionActivity[c] - millis()) > 20000ul) clients = true;
        }
        if(!clients){
          stage = 5;
          IP = false;
        }

      }
      else if((unsigned long)(millis() - wait_start) > MAX_RESP_TIME){
        wait_start = 0;
        if(!search("OK\r\n")){
          need_restart = true;
        }
      }
      else if(parse_P((const char *)F("AT\r\r\n\r\nERROR\r\n")) || parse_P((const char *)F("ERROR\r\n"))){
        wait_start = 0;
        need_restart = true;
        Serial.println(F("fail"));
        printRecBuffer();
        clearBuffer();
      }
    }
    else{
      Serial1.println(F("AT"));
      Serial.print(F("Act?"));
      wait_start = millis() | 1;
    }
  }

  int new_client, new_json;
  if(parse_P((const char *)F("+IPD,%d,%d:"),&new_client,&new_json)){
    recClient = new_client;
    json_leng = new_json;
    receiving = true;
    Serial.print(F("msg from "));
    Serial.print(recClient);
    Serial.print(',');
    Serial.print(json_leng);
    last_activity = millis();
  }
  if(receiving){
    json_leng = recv(json[recClient], json_leng, jsonBufferSize);
    if(!json_leng){
      Serial.println(F(":succ"));      
      receiving = false;
      recClient = -1;
      connectionActivity[new_client] = millis();
    }
  }
  
  if(parse_P((const char *)F("%d,CONNECT\r\n"),&new_client)){
    connections[new_client] = 0;
    connectionKey[new_client] = -1;
    connectionActivity[new_client] = millis();
    Serial.print(F("Client connected:"));
    Serial.println(new_client);
    last_activity = millis();
  }
  if(parse_P((const char *)F("%d,CLOSED\r\n"),&new_client)){
    connections[new_client] = -1;
    connectionActivity[new_client] = 0;
    Serial.print(F("Client unconnected:"));
    Serial.println(new_client);
    json[new_client][0] = '\0';
    last_activity = millis();
  }
  if(parse_P((const char *)F("%d,CONNECT FAIL\r\n"),&new_client)){
    connections[new_client] = -1;
    connectionActivity[new_client] = 0;
    Serial.print(F("Client failed:"));
    Serial.println(new_client);
    json[new_client][0] = '\0';
    last_activity = millis();
  }
  if(parse_P((const char *)F("WIFI CONNECTED\r\n"))){
    Serial.println(F("WIFI CONNECTED"));
    last_activity = millis();
  }
  if(parse_P((const char *)F("WIFI GOT IP\r\n"))){
    Serial.println(F("WIFI GOT IP"));
    last_activity = millis();
  }
  if(parse_P((const char *)F("WIFI DISCONNECT\r\n")) || parse_P((const char *)F("WIFI DISCONNECTED\r\n"))){
    for(int c = 0; c < clientMax; c++){
      connections[c] = -1;
      json[c][0] = '\0';
    }
    Serial.println(F("WIFI DISCONNECTED"));
    last_activity = millis();
  }
  if(parse_P((const char *)F("busy p..."))){
    Serial.println(F("###busy###"));
  }
}
