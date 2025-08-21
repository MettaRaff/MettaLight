#ifndef Settings_h
#define Settings_h

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

// VALUES
#define BRIGHTMIN 10 // 0-100 минимальная яркость при регулировке с кнопки
// VALUES

struct Data
{
    char SSID[32] = "";
    char pass[32] = "";
};

#endif