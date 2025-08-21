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