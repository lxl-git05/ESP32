#include "Initial.h"

void Initial(void)
{
    OLED_Init();
    LED_Init();
    Timer_Init();
    Key_Init();
    PWM_Init();
    Wifi_Init();    // WIFI
}