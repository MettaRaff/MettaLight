#ifndef PowerControl_h
#define PowerControl_h

#include "LIBS.h"
#include "Settings.h"

void PowerControl(bool pwr);
void PowerWhiteControl(bool pwr);
void PowerWake();
void BrightControl();
void BrightEndAnim();

extern uint32_t Timer_pwr;
extern uint8_t MODE;
extern uint8_t PowerMode, PowerWhiteMode;
extern bool swCol, swWork;
extern int16_t sld_br, sld_wbr;
extern boolean lastBright;
extern uint8_t BrightMin;
extern uint32_t BrightTimer;
extern uint8_t endAnimMode;
extern boolean BrightEndHolded;
extern boolean brightDir[];

extern ADRGB mystrip;

#endif