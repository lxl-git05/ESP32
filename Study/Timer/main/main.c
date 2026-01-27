#include <stdio.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "MyLED.h"
#include "OLED.h"
#include "Timer.h"

int check = 0 ;

void app_main(void)
{
    OLED_Init();
    LED_Init();
    Timer_Init();
    while (1)
    {
        //OLED
        OLED_Clear();
        OLED_ShowChar(0, 0, check ++, OLED_8X16);
        OLED_ShowString(0, 16, "Hello World!", OLED_6X8);
        OLED_ShowNum(0, 32, 123456, 6, OLED_8X16);
        OLED_ShowSignedNum(0, 48, -123456, 6, OLED_8X16);
        OLED_Update();

        ESP_LOGI("main", "conut: %d", check);
        vTaskDelay(pdMS_TO_TICKS(1000));
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
}
