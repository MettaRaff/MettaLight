#include <Arduino.h>
#include "init.h"
#include <ESP8266WiFi.h>
#include <AdRGB.h>
#include <GyverButton.h>
// #include <EEPROM.h>
#include <SimplePortal.h>
// #include <GyverHub.h>
#include <GyverPortal.h>
#include <GyverOS.h>
#include <EEManager.h>

// PINOUT
#define REDOUT 12       // красный пин ленты
#define GREENOUT 13     // зеленый пин ленты
#define BLUEOUT 14      // синий пин ленты
#define WHITEOUT 15     // пин рабочего света
#define BTN_PIN 4       // кнопка подключена сюда (BTN_PIN --- КНОПКА --- GND)
#define BTN_LED_PIN 5   // светодиод кнопки
#define USE_BUTTON true // распаяна кнопка?
#define DELAY_CONNECT 30000
#define SWITCH_BUTTON false
// PINOUT

// TIME PARAMS
#define TIME_ON 1000
#define TIME_OFF 2000
#define MAIN_SMOOTH 1000
// TIME PARAMS

// SYSTEM PARAMS
#define GH_ASYNC true
// SYSTEM PARAMS

ADRGB mystrip(REDOUT, GREENOUT, BLUEOUT, WHITEOUT);
GButton butt1(BTN_PIN);
GyverOS<5> OS;
// gh::Color col(gh::Colors::Aqua);
// GyverHub hub("MyDevices", "MettaLight", "");
GyverPortal ui;

// GHbutton b1, b2;
bool swPWR = 1, swCol = 1, swWork = 0, swRand = 0;
int16_t sld_br = 100, sld_wbr = 100;
uint8_t sld_hue = 160;
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

EEManager memory(data);

void build()
{
  GP.BUILD_BEGIN(GP_DARK);

  GP.TITLE("MettaLed", "t1");
  GP.HR();

  GP.LABEL("Power: ");
  GP.SWITCH("power", swCol); // GP.BREAK();
  GP.LABEL("Work: ");
  GP.SWITCH("work", swWork);
  GP.BREAK();

  GP.LABEL("Bright: ");
  GP.SLIDER("slCol", sld_br, 0, 255);
  GP.BREAK();
  GP.LABEL("White: ");
  GP.SLIDER("slWk", sld_wbr, 0, 255);
  GP.BREAK();

  GP.NAV_TABS_M("tbs", "RANDOM,FIRE,COLOR");

  GP.NAV_BLOCK_BEGIN("tbs", 0);
  GP.LABEL("Smooth: ");
  GP.SLIDER("ranSmth", sld_ran, 0, 100);
  GP.BREAK();
  GP.NAV_BLOCK_END();

  GP.NAV_BLOCK_BEGIN("tbs", 1);
  GP.LABEL("Hue");
  GP.SLIDER("slHue", sld_hue, 0, 255);
  GP.BREAK();
  GP.NAV_BLOCK_END();

  GP.NAV_BLOCK_BEGIN("tbs", 2);
  GP.LABEL("Color");
  GP.COLOR("colr", valCol);
  GP.NAV_BLOCK_END();

  GP.BUILD_END();
}

void setup()
{
  devicesInit();

  if (!digitalRead(BTN_PIN))
  {
    memory.reset();
    mystrip.setAccel(255, 0, 0);
    mystrip.tick();
    setup_portal();

    // strcpy(data.SSID, "Quickness");
    // strcpy(data.pass, "Nesquik666");
    // memory.updateNow();
    // ESP.reset();
  }
  else
  {
    // #ifdef GH_ESP_BUILD
    // EEPROM.get(0, portalCfg); // достать параметры сети wifi из eeprom

    Serial.print("SSID: ");
    Serial.println(data.SSID);
    Serial.print("pass: ");
    Serial.println(data.pass);
    WiFi.mode(WIFI_STA);
    WiFi.begin(data.SSID, data.pass);

    Timer_2 = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
      if (millis() - Timer_2 > DELAY_CONNECT)
      {
        mystrip.setAccelHEX(GP_PINK_B);
        flag_ap = true;
        break;
      }
    }

    if (flag_ap)
      setup_portal();

    Serial.println();
    Serial.println(WiFi.localIP());

    // #endif
  }
  OS.attach(0, lightProcessor, LightProcStep);
  OS.attach(1, butEvents, 20);

  ui.attachBuild(build);
  ui.attach(action);
  ui.start("mettaled");
}

void loop()
{
  OS.tick();
  mystrip.tick();
  ui.tick();
}

void action()
{
  // был клик по компоненту
  if (ui.click())
  {
    // проверяем компоненты и обновляем переменные
    // Serial.println(ui.clickName());
    // 1. переписали вручную
    /*if (ui.click("ch")) {
      valCheck = ui.getBool("ch");
      Serial.print("Check: ");
      Serial.println(valCheck);
    }*/

    // 2. автоматическое обновление переменной
    if (ui.clickBool("power", swCol))
    {
      Serial.print("Power: ");
      Serial.println(swCol);
      PowerControl(swCol);
    }

    if (ui.clickBool("work", swWork))
    {
      Serial.print("Work: ");
      Serial.println(swWork);
      PowerWhiteControl(swWork);
    }

    if (ui.clickInt("slCol", sld_br))
    {
      Serial.print("Bright: ");
      Serial.println(sld_br);
      mystrip.setBrightSmooth(sld_br, MAIN_SMOOTH);
      PowerWake();
    }

    if (ui.clickInt("slWk", sld_wbr))
    {
      Serial.print("White: ");
      Serial.println(sld_wbr);
      mystrip.setBrightWSmooth(sld_wbr, MAIN_SMOOTH);
      if (sld_wbr > 0)
        swWork = true;
    }

    if (ui.clickInt("slHue", sld_hue))
    {
      Serial.print("Hue: ");
      Serial.println(sld_hue);
    }

    if (ui.clickName().startsWith("tbs/"))
    {
      String index = ui.clickName();
      index.remove(0, 4);
      MODE = index.toInt();
      Serial.print("Mode: ");
      Serial.println(MODE);
    }
    /*
        if(ui.click("tbs/0")){
          MODE = 0;
          Serial.print("Mode: ");
          Serial.println(MODE);
        }

        if(ui.click("tbs/1")){
          MODE = 1;
          Serial.print("Mode: ");
          Serial.println(MODE);
        }

        if(ui.click("tbs/2")){
          MODE = 2;
          Serial.print("Mode: ");
          Serial.println(MODE);
        }*/

    if (ui.clickInt("ranSmth", sld_ran))
    {
      Serial.print("Smooth: ");
      Serial.println(sld_ran);
      // randBow_time = float(randBow_time * 2) * float(sld_ran) / 100.0;
      randBow_time = sld_ran * 20;
    }

    if (ui.clickColor("colr", valCol))
    {
      Serial.print("Color: ");
      Serial.println(valCol.encode());
      mystrip.setMoveHEX(valCol.getHEX(), MAIN_SMOOTH);
      // mystrip.setMoveCol(0, valCol.g, MAIN_SMOOTH);
      // mystrip.setMoveCol(1, valCol.r, MAIN_SMOOTH);
      // mystrip.setMoveCol(2, valCol.b, MAIN_SMOOTH);
    }

    /*if (ui.clickString("txt", valText)) {
      Serial.print("Text: ");
      Serial.println(valText);
    }

    if (ui.clickInt("num", valNum)) {
      Serial.print("Number: ");
      Serial.println(valNum);
    }

    if (ui.clickStr("pass", valPass)) {
      Serial.print("Password: ");
      Serial.println(valPass);
    }

    if (ui.clickFloat("spn", valSpin)) {
      Serial.print("Spinner: ");
      Serial.println(valSpin);
    }

    if (ui.clickDate("date", valDate)) {
      Serial.print("Date: ");
      Serial.println(valDate.encode());
    }

    if (ui.clickTime("time", valTime)) {
      Serial.print("Time: ");
      Serial.println(valTime.encode());
    }

    if (ui.clickInt("sel", valSelect)) {
      Serial.print("Select: ");
      Serial.println(valSelect);
    }
    if (ui.clickInt("rad", valRad)) {
      Serial.print("Radio: ");
      Serial.println(valRad);
    }

    if (ui.click("btn")) Serial.println("Button click");
    */
  }
}

void devicesInit()
{
  Serial.begin(115200);
  EEPROM.begin(1024);

  valCol.setHEX(0x7366bd);

  byte stat = memory.begin(0, 'b');

  Serial.println(stat);

  mystrip.HighFrequency(true);
  mystrip.setMaxBright(99);

  mystrip.setAccel(0, 0, 200);
  mystrip.tick();

  if (USE_BUTTON)
  {
    pinMode(BTN_LED_PIN, OUTPUT);
    analogWrite(BTN_LED_PIN, 255);
    digitalWrite(BTN_LED_PIN, true);

    butt1.setDebounce(50);      // настройка антидребезга (по умолчанию 80 мс)
    butt1.setTimeout(300);      // настройка таймаута на удержание (по умолчанию 500 мс)
    butt1.setClickTimeout(600); // настройка таймаута между кликами (по умолчанию 300 мс)
    butt1.setType(HIGH_PULL);
    butt1.setDirection(NORM_OPEN);
  }

  //mystrip.setMove(0, 0, 200, MAIN_SMOOTH);
  //mystrip.tick();
}

void setup_portal()
{
  Serial.println("HOLD, AP MODE");

  mystrip.setAccelHEX(GP_YELLOW);
  mystrip.tick();
  portalRun(300000); // запустить с таймаутом 60с
  // portalRun(30000); // запустить с кастомным таймаутом

  Serial.println(portalStatus());
  // статус: 0 error, 1 connect, 2 ap, 3 local, 4 exit, 5 timeout

  if (portalStatus() == SP_SUBMIT)
  {
    strcpy(data.SSID, portalCfg.SSID);
    strcpy(data.pass, portalCfg.pass);
    memory.updateNow();

    Serial.println(data.SSID);
    Serial.println(data.pass);
    memory.updateNow();
    // забираем логин-пароль
  }
  ESP.reset();
}

void lightProcessor()
{
  Timer_Light_Proc = millis();
  switch (PowerMode)
  {
  case 0:
    if (mystrip.getBlue() > 0 || mystrip.getRed() > 0 || mystrip.getGreen() > 0)
      mystrip.setAccel(0, 0, 0, 0);
    break;
  case 1:
    switch (MODE)
    {
    case 0:
      randBow_Mode();
      break;
    case 1:
      classicFire_Mode();
      break;
    case 2:
      randCol_Mode();
      break;
    }

    break;
  case 2:
    if (millis() - Timer_pwr > TIME_ON)
      PowerMode = 1;
    break;
  case 3:
    if (millis() - Timer_pwr > TIME_OFF)
      PowerMode = 0;
    break;
  case 4:
    BrightEndAnim();
  }
}

void butEvents()
{
  butt1.tick();

  if (butt1.isSingle())
  {
    lastBright = 0;
    if (PowerMode == 0)
      PowerControl(1);
    else
      PowerControl(0);
  }
  if (butt1.isDouble())
  {
    lastBright = 1;
    if (PowerWhiteMode == 0)
      PowerWhiteControl(1);
    else
      PowerWhiteControl(0);
  }
  if (butt1.isHold())
  {
    if (PowerMode == 1)
      BrightControl();
  }
}

void BrightControl()
{
  static boolean brightDir;
  if (!brightDir)
  {
    if (lastBright == false)
    {
      sld_br--;
      if (sld_br == BrightMin)
      {
        brightDir = true;
        BrightTimer = millis();
        PowerMode = 4;
      }
      mystrip.setBright(sld_br);
    }
    else
    {
      sld_wbr--;
      if (sld_wbr == BrightMin)
      {
        brightDir = true;
        BrightTimer = millis();
        PowerMode = 4;
      }
      mystrip.setBrightW(sld_wbr);
    }
  }
  else
  {
    if (lastBright == false)
    {
      sld_br++;
      if (sld_br >= 254)
      {
        brightDir = false;
        BrightTimer = millis();
        PowerMode = 4;
      }
      mystrip.setBright(sld_br);
    }
    else
    {
      sld_wbr++;
      if (sld_wbr >= 254)
      {
        brightDir = false;
        BrightTimer = millis();
        PowerMode = 4;
      }
      mystrip.setBrightW(sld_wbr);
    }
  }
}

void BrightEndAnim()
{
  uint32_t calc = millis() - BrightTimer;
  if (calc > 10 && calc < 20)
    mystrip.setBright(100);
    mystrip.setAccel(255, 255, 255);
  if (calc > 400 && calc < 410)
    mystrip.setAccel(0, 0, 0);
  if (calc > 800 && calc < 810)
    mystrip.setAccel(255, 255, 255);
  if (calc > 1000)
  {
    mystrip.setAccel(0, 0, 0);
    PowerMode = 1;
    mystrip.setBright(sld_br);
  }
}

void PowerControl(bool pwr)
{
  Timer_pwr = millis();
  if (swCol != pwr)
  {
    swCol = pwr;
    // hub.sendUpdate("power");
  }

  if (!pwr)
  {
    PowerMode = 3;
    // PowerWhiteMode = 0;
    mystrip.setMove(0, 0, 0, TIME_OFF);
    // hub.sendUpdate("work");
  }
  else
  {
    PowerMode = 2;
    if (MODE == 2)
      mystrip.setMoveHEX(valCol.getHEX(), TIME_ON);
  }
}

void PowerWake()
{
  if (PowerMode != 1)
  {
    swCol = true;
    PowerControl(swCol);
  }
}

void PowerWhiteControl(bool pwr)
{
  if (swWork != pwr)
  {
    swWork = pwr;
    // hub.sendUpdate("work");
  }
  if (!pwr)
  {
    mystrip.setMoveW(0, TIME_OFF);
    PowerWhiteMode = 0;
  }
  else
  {
    mystrip.setBrightWSmooth(sld_wbr, TIME_ON);
    PowerWhiteMode = 1;
  }
}

void randBow_Mode()
{
  if (millis() - Timer_1 > randBow_time)
  {
    mystrip.setMove(random(0, 255), random(0, 255), random(0, 255), randBow_time);
    Timer_1 = millis();
  }
}

void classicFire_Mode()
{
  static byte fireRnd = 0;
  static float fireValue = 0;

  if (millis() - Timer_1 > 200)
  {
    Timer_1 = millis();
    fireRnd = random(0, 10);
  }

  fireValue = (float)fireValue * (1 - 0.02) + (float)fireRnd * 10 * 0.02;

  uint8_t hue = constrain(map(fireValue, 20, 60, sld_hue, sld_hue + 60), 0, 255);
  uint8_t sat = constrain(map(fireValue, 20, 60, 255, 240), 0, 255);
  uint8_t val = constrain(map(fireValue, 20, 60, 100, 255), 0, 255);

  mystrip.setMoveHSV(hue, sat, val, 100);
}

void randCol_Mode()
{
  /*if (swRand)
  {
    if (offset > 0)
    {
      if (millis() - Timer_1 > randCol_Time)
      {
        Timer_1 = millis();

        int crgb[3];
        crgb[0] = (col >> 16) & 0xff;
        crgb[1] = (col >> 8) & 0xff;
        crgb[2] = col & 0xff;

        for (int i = 0; i < 3; i++)
        {
          crgb[i] += random(offset * -1, offset);
          crgb[i] = constrain(crgb[i], 0, 255);
        }
        mystrip.setMove(crgb[0], crgb[1], crgb[2], randCol_Time);
      }
    }
  }*/
}