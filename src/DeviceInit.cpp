#include "DeviceInit.h"

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

    ui.attachBuild(build);
    ui.attach(action);
    ui.start("mettaled");
}