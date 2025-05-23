#include <ESP8266WiFi.h>

void devicesInit();
void setup_portal();

void butEvents();
void lightProcessor();

void randBow_Mode();
void classicFire_Mode();
void randCol_Mode();
void colrMusic_Mode();

void PowerControl(bool pwr);
void PowerWhiteControl(bool pwr);
void PowerWake();
void BrightControl();
void BrightEndAnim();
void SerialCall();

void action();
void build();

uint8_t LightProcStep = 32;
uint8_t PowerMode = 2; // 0 выкл, 1 вкл, 2 включение, 3 выключение
uint8_t PowerWhiteMode = 0;
uint8_t MODE = 1;
uint32_t BrightTimer;
uint8_t BrightMin = 10;

int intsCM[5];

boolean flag_ap = false, flag_load = false;

boolean lastBright = 0;

uint32_t randBow_time = 1000, randCol_Time = 500;

uint32_t Timer_Light_Proc, Timer_pwr, Timer_1, Timer_2;

struct Data
{
    char SSID[32] = "";
    char pass[32] = "";
};

Data data;