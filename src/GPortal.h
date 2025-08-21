#ifndef GPortal_h
#define GPortal_h

#include "LIBS.h"
#include "Settings.h"

extern bool swCol, swWork;
extern int16_t sld_br, sld_wbr;
extern uint8_t sld_ran, sld_hue, sld_scale, sld_smth, sld_time;
extern float fireKoeff;
extern uint8_t MODE;
extern uint32_t randBow_time;

extern GPcolor valCol;
extern GyverPortal ui;
extern ADRGB mystrip;

extern void PowerControl(bool pwr);
extern void PowerWhiteControl(bool pwr);
extern void PowerWake();

void build();
void action();

#endif