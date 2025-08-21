#include "init.h"

void setup()
{
  devicesInit();

  if (!digitalRead(BTN_PIN)) // если удерживается кнопка при запуске
  {
    memory.reset();
    mystrip.setAccel(255, 0, 0);
    mystrip.tick();
    setup_portal(); // запустить модем для внесения настройки wifi
  }
  else // обычный старт
  {
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
      if (millis() - Timer_2 > DELAY_CONNECT) // если не удалось подключиться в течении тайм-аута
      {
        mystrip.setAccelHEX(GP_PINK_B);
        flag_ap = true; // поднять флаг чтоб снова запустить модем
        break;
      }
    }

    if (flag_ap)
      setup_portal(); // запустить модем для внесения настройки wifi

    Serial.println();
    Serial.println(WiFi.localIP());

    // #endif
  }
  OS.attach(0, lightProcessor, LightProcStep);
  OS.attach(1, butEvents, 20);
  OS.attach(2, SerialCall, 20);
  // OS.attach(3, checkUi, 1000);

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

  // mystrip.setMove(0, 0, 200, MAIN_SMOOTH);
  // mystrip.tick();
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

void checkUi()
{
  Serial.print("UI state: ");
  Serial.print(WiFi.status());
  Serial.println(ui.state());
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
    if (PowerMode == 1)
      BrightControl();
  }
}

void BrightControl()
{
  static boolean brightDir;

  // Serial.print("Bright state: ");
  // Serial.print(brightDir);
  // Serial.print(lastBright);
  // Serial.print(sld_br);
  // Serial.println(sld_wbr);

  if (!brightDir) // на понижение яркости
  {
    if (lastBright == false) // последним был цветной
    {
      sld_br--;
      if (sld_br <= BrightMin)
      {
        sld_br = BrightMin;
        brightDir = true;
        BrightTimer = millis();
        PowerMode = 4;
        endAnimMode = 1;
        Serial.println("Min end of color");
      }
      mystrip.setBright(sld_br);
    }
    else // последним был белый
    {
      sld_wbr--;
      if (sld_wbr == BrightMin)
      {
        brightDir = true;
        BrightTimer = millis();
        PowerMode = 4;
        endAnimMode = 3;
      }
      mystrip.setBrightW(sld_wbr);
    }
  }
  else // на повышение яркости
  {
    if (lastBright == false) // последним был цветной
    {
      sld_br++;
      if (sld_br >= 99)
      {
        brightDir = false;
        BrightTimer = millis();
        PowerMode = 4;
        endAnimMode = 0;
      }
      mystrip.setBright(sld_br);
    }
    else // последним был белый
    {
      sld_wbr++;
      if (sld_wbr >= 99)
      {
        brightDir = false;
        BrightTimer = millis();
        PowerMode = 4;
        endAnimMode = 2;
      }
      mystrip.setBrightW(sld_wbr);
    }
  }
}

void BrightEndAnim()
{
  // index:
  // 0 - max color
  // 1 - min color
  // 2 - max white
  // 3 - min white
  uint32_t calc = millis() - BrightTimer;
  Serial.print("Bright anim func! calc time: ");
  Serial.println(calc);
  if (calc > 10 && calc < 20)
  {
    mystrip.setBright(100);
    if (swWork) mystrip.setBrightW(0);
    if (endAnimMode == 0 || endAnimMode == 2)
      mystrip.setAccel(255, 255, 255);
    if (endAnimMode == 1 || endAnimMode == 3)
      mystrip.setAccel(255, 0, 0);
  }
  /*if (calc > 400 && calc < 410)
    mystrip.setAccel(0, 0, 0);
  if (calc > 800 && calc < 810)
    mystrip.setAccel(255, 255, 255);*/
  if (calc > 1000)
  {
    mystrip.setAccel(0, 0, 0);
    PowerMode = 1;
    mystrip.setBright(sld_br);
    if (swWork) mystrip.setBrightW(sld_wbr);
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

  if (millis() - Timer_1 > sld_time)
  {
    Timer_1 = millis();
    fireRnd = random(0, 10);
  }

  fireValue = (float)fireValue * (1 - fireKoeff) + (float)fireRnd * sld_scale * fireKoeff;

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

void colrMusic_Mode()
{
  mystrip.setMove(intsCM[0], intsCM[1], intsCM[2], 20);
  mystrip.setMoveW(intsCM[3], 20);
}