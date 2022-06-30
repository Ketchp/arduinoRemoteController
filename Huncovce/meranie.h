#ifndef MERANIE_HEADER
#define MERANIE_HEADER

void initInput();
void meranie();
extern const uint8_t temperaturePin;

extern int otacky_1; //RMP
extern int otacky_2; // porucha -> -1
extern volatile unsigned int tg1_rot;
extern volatile unsigned int tg2_rot;
extern long realtime;


extern float teplota; //degC
extern float teplota_clock;
extern bool dialkove_ovladanie;
extern bool manualne_ovladanie;
extern bool not_manualne;
extern uint8_t stavidla_poloha; //0dole 1stred 2hore
extern bool cistenie_pretazenie;
extern bool cistenie_otocka;
extern volatile bool cistenieActive;
extern bool cistenieEnabled;
extern bool cistenieDeblokEnabled;
extern bool cleaning;
extern bool tg1_riadenie_enabled;
extern bool tg2_riadenie_enabled;
extern bool alwaysTrue;
extern volatile bool manual_cistenie;
extern volatile bool auto_cistenie;
extern bool sietovaOchrana;
extern bool ceslaOchrana;
extern bool stavidlaOchrana;

//control variables
extern volatile uint8_t cistenie; //0vzad 1stoj 2vpred
extern volatile uint8_t stavidla_ovladanie; //0nadol 1stoj 2nahor
extern uint8_t tg1; //0vzad 1stoj 2vpred
extern uint8_t tg2; //0vzad 1stoj 2vpred
extern bool tg1_ventil;
extern bool tg2_ventil;
extern bool tg1_kompenzacia;
extern bool tg2_kompenzacia;
extern bool kompDialkove;
extern bool tg1_cistenie;
extern bool tg2_cistenie;
extern bool tg1_priebezne;
extern bool tg2_priebezne;
extern uint8_t tg1_phase;
extern uint8_t tg2_phase;
extern uint8_t tg1_request;
extern uint8_t tg2_request;

//time keeping
extern unsigned long cistenie_zaciatok;
extern unsigned long stavidlo_zaciatok;
extern unsigned long kW1_pod;
extern unsigned long kW2_pod;
extern unsigned long tg1_t;
extern unsigned long tg2_t;
extern unsigned long tg1_moveStart;
extern unsigned long tg2_moveStart;
extern unsigned long last_display;
extern unsigned long last_temperature;
extern unsigned long last_time_upt;
extern bool tg1Led;
extern bool tg2Led;


//EEPROM values
extern uint16_t T_cistenie; //min
extern float min_kW1; //kW
extern float min_kW2;
extern long posledne_cistenie; //unix s
extern float cistenie_t; //s
extern float stavidla_ovladanie_t; //s
extern float perioda_cistenie_tg; //h
extern long tg1_last_clean; //unix s
extern long tg2_last_clean; //unix s
extern uint16_t reset_count;
extern uint16_t turnOnAddress;
extern long lastRestart;

//EEPROM addresses
extern const uint16_t T_cistenie_eeAdd;
extern const uint16_t min_kW1_eeAdd;
extern const uint16_t min_kW2_eeAdd;
extern const uint16_t posledne_cistenie_eeAdd;
extern const uint16_t cistenie_t_eeAdd;
extern const uint16_t stavidla_ovladanie_t_eeAdd;
extern const uint16_t perioda_cistenie_tg_eeAdd;
extern const uint16_t tg1_last_clean_eeAdd;
extern const uint16_t tg2_last_clean_eeAdd;
extern const uint16_t reset_count_eeAdd;
extern const uint16_t turnOnAddress_eeAdd;



#endif
