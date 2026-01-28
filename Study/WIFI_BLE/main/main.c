#include <stdio.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "Timer_Counter.h"

#include "MyLED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
#include "PWM.h"
#include "Wifi_Manager.h"   // WIFI

void app_main(void)
{
    OLED_Init();
    LED_Init();
    Timer_Init();
    Key_Init();
    PWM_Init();
    Wifi_Init();    // WIFI

    while (1)
    {
        Timer_Counter_Func() ;
        //OLED显示
        OLED_Printf(0 , 0 , OLED_8X16 , "%s" , "Hello World");
        // Key
        if(Key_Check(KEY_0 , KEY_SINGLE))
        {
            printf("Key_0 SINGLE\n");
        }
        else if (Key_Check(KEY_0 , KEY_DOUBLE))
        {
            printf("Key_0 Double\n");
            Timer_Counter_Print();
        }
        // 功能计时区
        Timer_Counter_Begin();

        Timer_Counter_End();
        
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
        LED_Write(led_Status);
        led_Status = !led_Status ;
        tim_cnt = 0 ;
    }

    // 功能2:检测按键状态1ms周期
    Key_Tick();
}
