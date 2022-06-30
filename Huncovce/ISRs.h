#ifndef ISRS_HEADER
#define ISRS_HEADER

extern volatile uint8_t interr1;
extern volatile uint8_t interr2;
extern volatile uint32_t cP1;
extern volatile uint32_t cP2;
extern volatile uint32_t jP1;
extern volatile uint32_t jP2;
extern volatile uint16_t cC1;
extern volatile uint16_t jC1;
extern volatile uint16_t cC2;
extern volatile uint16_t jC2;
extern volatile bool newValues;

#endif
