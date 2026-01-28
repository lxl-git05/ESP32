#include <stdio.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "MyLED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"

int check = 0 ;

void app_main(void)
{
    OLED_Init();
    LED_Init();
    Timer_Init();
    Key_Init();
    while (1)
    {
        //OLED显示
        OLED_ShowString(0, 0, "Hello World!", OLED_8X16);
        // Key
        if(Key_Check(KEY_0 , KEY_SINGLE))
        {
            printf("Key_0 Single\n");
        }
        else if (Key_Check(KEY_0 , KEY_DOUBLE))
        {
            printf("Key_0 Double\n");
        }
        
        OLED_Update();
    }
}

// 定时器1ms中断
void Timer_Callback_1ms(void)
{
    static int tim_cnt = 0 ;
    tim_cnt ++ ;

    // 功能1: 1s 切换一次LED灯
    static int led_Status = 0 ;
    if (tim_cnt >= 1000)
    {
        led_Status = !led_Status ;
        tim_cnt = 0 ;
        LED_Write(led_Status);
    }

    // 功能2:检测按键状态,必须在20ms周期调用
    Key_Tick();
}
