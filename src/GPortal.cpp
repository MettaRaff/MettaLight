#include "GPortal.h"

void build()
{
    GP.BUILD_BEGIN(GP_DARK);
    GP.UPDATE("power,work,slCol,slWk");
    GP.TITLE("MettaLed", "t1");
    GP.HR();

    GP.LABEL("Power: ");
    GP.SWITCH("power", swCol);
    GP.LABEL("Work: ");
    GP.SWITCH("work", swWork);
    GP.BREAK();

    GP.LABEL("Bright: ");
    GP.SLIDER("slCol", sld_br, 0, 100);
    GP.BREAK();
    GP.LABEL("White: ");
    GP.SLIDER("slWk", sld_wbr, 0, 100);
    GP.BREAK();

    GP.NAV_TABS_M("tbs", "RANDOM,FIRE,COLOR,MUSIC");

    GP.NAV_BLOCK_BEGIN("tbs", 0);
    GP.LABEL("Smooth: ");
    GP.SLIDER("ranSmth", sld_ran, 0, 100);
    GP.BREAK();
    GP.NAV_BLOCK_END();

    GP.NAV_BLOCK_BEGIN("tbs", 1);
    GP.LABEL("Hue");
    GP.SLIDER("slHue", sld_hue, 0, 255);
    GP.BREAK();
    GP.LABEL("Scale");
    GP.SLIDER("scale", sld_scale, 1, 30); // дольность заброса цвета
    GP.BREAK();
    GP.LABEL("Soft");
    GP.SLIDER("slSmth", sld_smth, 2, 10); // сила сглаживания
    GP.BREAK();
    GP.LABEL("Speed");
    GP.SLIDER("slSpd", sld_time, 50, 300); // сила сглаживания
    GP.BREAK();
    GP.NAV_BLOCK_END();

    GP.NAV_BLOCK_BEGIN("tbs", 2);
    GP.LABEL("Color");
    GP.COLOR("colr", valCol);
    GP.NAV_BLOCK_END();

    GP.NAV_BLOCK_BEGIN("tbs", 3);
    GP.LABEL("Music");
    // GP.COLOR("colr", valCol);
    GP.NAV_BLOCK_END();

    GP.BUILD_END();
}

void action()
{
    // был клик по компоненту
    if (ui.click())
    {
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

        if (ui.clickInt("slSmth", sld_smth))
        {
            uint16_t bigKoeff = sld_smth * 25;
            fireKoeff = float(bigKoeff / 10000.0);
            Serial.print("Smooth: ");
            Serial.println(fireKoeff);
        }

        if (ui.clickName().startsWith("tbs/"))
        {
            String index = ui.clickName();
            index.remove(0, 4);
            MODE = index.toInt();
            Serial.print("Mode: ");
            Serial.println(MODE);
        }

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
        }

        if (ui.update())
        {
            ui.updateInt("slCol", sld_br);
            ui.updateInt("slWk", sld_wbr);
            ui.updateBool("power", swCol);
            ui.updateBool("work", swWork);
        }
    }
}
