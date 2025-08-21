#include "Modes.h"

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