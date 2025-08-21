#ifndef DeviceInit_h
#define DeviceInit_h

#include "LIBS.h"
#include "Settings.h"

extern GPcolor valCol;
extern ADRGB mystrip;
extern GButton butt1;
extern EEManager memory;
extern Data data;
extern uint32_t Timer_2;
extern boolean flag_ap;
extern GyverOS<4> OS;
extern GyverPortal ui;
extern uint8_t LightProcStep;

extern void setup_portal();
extern void lightProcessor();
extern void butEvents();
extern void SerialCall();
extern void build();
extern void action();

void devicesInit();

#endif