#include "init.h"

void setup()
{
  devicesInit();
}

void loop()
{
  OS.tick();
  mystrip.tick();
  ui.tick();
}