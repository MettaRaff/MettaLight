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
    mystrip.setBrightSmooth(0, TIME_OFF);
  }
  else
  {
    mystrip.setBrightSmooth(sld_br, TIME_ON);
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

// вычисляет новую яркость при удержании кнопки
void BrightControl()
{
  if (!lastBright && brightDir[0])
    sld_br++; // режим яркости цвета в +
  if (!lastBright && !brightDir[0])
    sld_br--; // режим яркости цвета в -
  if (lastBright && brightDir[1])
    sld_wbr++; // режим яркости цвета в +
  if (lastBright && !brightDir[1])
    sld_wbr--; // режим яркости цвета в -

  if (!lastBright && sld_br == BrightMin)
  {
    BrightEndHolded = true;
    brightDir[0] = true;
    endAnimMode = 1;
  }

  if (!lastBright && sld_br == 100)
  {
    BrightEndHolded = true;
    brightDir[0] = false;
    endAnimMode = 0;
  }

  if (lastBright && sld_wbr == BrightMin)
  {
    BrightEndHolded = true;
    brightDir[1] = true;
    endAnimMode = 3;
  }

  if (lastBright && sld_wbr == 100)
  {
    BrightEndHolded = true;
    brightDir[1] = false;
    endAnimMode = 2;
  }

  if (!lastBright)
    mystrip.setBright(sld_br);
  if (lastBright)
    mystrip.setBrightW(sld_wbr);

  if (BrightEndHolded)
  {                         // флаг что дошли до предела, пока кнопка не отпустится не менять яркость
    BrightTimer = millis(); // обнуляем таймер для анимации края
    PowerMode = 4;          // меняем режим с обычного на проигр анимации края
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
  if (calc > 0 && calc < 40)
  {
    Serial.print("EndAnimMode: ");
    Serial.println(endAnimMode);
    mystrip.setBright(100);
    if (endAnimMode == 0 || endAnimMode == 2)
      mystrip.setAccel(0, 255, 0);
    if (endAnimMode == 1 || endAnimMode == 3)
      mystrip.setAccel(255, 0, 0);
  }

  if (calc >= 900 && calc < 940)
  {
    Serial.println("It dis signal block!");
    mystrip.setAccel(0, 0, 0);
    mystrip.setBright(sld_br);
    // if (swWork) mystrip.setBrightW(sld_wbr);
  }

  if (calc > 1000)
  {
    Serial.println("It exit signal block!");
    PowerMode = 1;
  }
}