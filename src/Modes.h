#ifndef Modes_h
#define Modes_h

#include "LIBS.h"

extern uint32_t Timer_1;
extern uint32_t randBow_time;
extern uint8_t sld_hue, sld_scale, sld_time;
extern float fireKoeff;
extern int intsCM[];

extern ADRGB mystrip;

void randBow_Mode();
void classicFire_Mode();
void randCol_Mode();
void colrMusic_Mode();

#endif