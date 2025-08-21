#include "LIBS.h"
#include "Settings.h"
#include "GPortal.h"

void devicesInit();
void setup_portal();

void butEvents();
void checkUi();
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

struct Data
{
    char SSID[32] = "";
    char pass[32] = "";
}data;

ADRGB mystrip(REDOUT, GREENOUT, BLUEOUT, WHITEOUT);
GButton butt1(BTN_PIN);
GyverOS<4> OS;
GyverPortal ui;

bool swPWR = 1, swCol = 1, swWork = 0, swRand = 0;
int16_t sld_br = 100, sld_wbr = 100;
uint8_t sld_hue = 160, sld_scale = 10, sld_smth = 5, sld_time = 200;
float fireKoeff = 0.02;
uint8_t offset = 35;
uint8_t sld_ran = 50;

// переменные
bool valCheck;
bool valSwitch;
String valText;
int valNum;
char valPass[10];
float valSpin;
int valSlider;
GPdate valDate;
GPtime valTime;
GPcolor valCol;
int valSelect;
int valRad;
uint8_t endAnimMode = 0;

EEManager memory(data);

uint8_t LightProcStep = 32; // частота обработки света
uint8_t PowerMode = 2; 
// 0 выкл, 1 вкл, 2 включение, 3 выключение, 4 end anim
uint8_t PowerWhiteMode = 0;
uint8_t MODE = 1;
uint32_t BrightTimer;
uint8_t BrightMin = 10;

int intsCM[5];

boolean flag_ap = false, flag_load = false;

boolean lastBright = 0;

uint32_t randBow_time = 1000, randCol_Time = 500;

uint32_t Timer_Light_Proc, Timer_pwr, Timer_1, Timer_2;