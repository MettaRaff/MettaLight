#ifndef CoreCycle_h
#define CoreCycle_h

#include "LIBS.h"
#include "Settings.h"

extern uint32_t Timer_Light_Proc, Timer_pwr;
extern uint8_t PowerMode, PowerWhiteMode;
extern uint8_t MODE;
extern boolean lastBright;
extern int intsCM[];
extern Data data;

extern ADRGB mystrip;
extern GButton butt1;
extern EEManager memory;

extern void randBow_Mode();
extern void classicFire_Mode();
extern void randCol_Mode();
extern void colrMusic_Mode();
extern void BrightEndAnim();
extern void PowerControl(bool pwr);
extern void PowerWhiteControl(bool pwr);
extern void BrightControl();

void lightProcessor();
void butEvents();
void SerialCall();
void setup_portal();

#endif