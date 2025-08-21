#include "PowerControl.h"

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
    PowerMode = 3; // выключение
    PowerWhiteControl(0);
    mystrip.setMove(0, 0, 0, TIME_OFF);
  }
  else
  {
    if (MODE != 2)
    {
      PowerMode = 1;
    }
    else
      PowerMode = 2;
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

void PowerWake()
{
  if (PowerMode != 1)
  {
    swCol = true;
    PowerControl(swCol);
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

  if (calc > 1000)
  {
    mystrip.setAccel(0, 0, 0);
    PowerMode = 1;
    mystrip.setBright(sld_br);
    if (swWork) mystrip.setBrightW(sld_wbr);
  }
}