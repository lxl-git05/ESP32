#include <Arduino.h>
#include "LED.h"

// LED库.c .h
void LED_Init(void)
{
    pinMode(LED_BUILTIN, OUTPUT);
    LED_On() ;  // 默认打开LED
}

void LED_On(void)
{
    digitalWrite(LED_BUILTIN, LOW);
}

void LED_Off(void)
{
    digitalWrite(LED_BUILTIN, HIGH);
}
