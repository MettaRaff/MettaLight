#include "CoreCycle.h"

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
    case 3:
      colrMusic_Mode();
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
    break;
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
    // PowerWhiteControl(0);
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
    if (PowerMode == 1 && !BrightEndHolded)
      BrightControl();
  }
  if (butt1.isRelease()) BrightEndHolded = false;
}

void SerialCall()
{
  if (Serial.available())
  {
    char buf[16];
    Serial.readBytesUntil(';', buf, 16);
    Parser data(buf, ',');
    data.parseInts(intsCM);
  }
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