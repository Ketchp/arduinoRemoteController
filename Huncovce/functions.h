#ifndef FUNCTIONS_HEADER
#define FUNCTIONS_HEADER
#include <stdarg.h>

void tg1_increment();
void tg2_increment();
void calculatePower();
void setup_WDT();
void setup_ADC();
void setup_timer();
void print(uint8_t channel, const char* format, ...);
void println(uint8_t channel, const char* format, ...);
extern float vykon1; //kW
extern float vykon2;
extern float cos1;
extern float cos2;


#endif
