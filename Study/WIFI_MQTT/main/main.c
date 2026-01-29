#include <stdio.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "stdbool.h"

#include "Timer_Counter.h"

#include "MyLED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
#include "PWM.h"
#include "Wifi_Manager.h"   // WIFI
#include "Wifi_MQTT.h"

char Command[64] = {0} ;
int LED_PWM = 0 ;
bool LED_Status ;

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
            ESP_ERROR_CHECK( Send_To_Onenet());
            
        }
        else if (Key_Check(KEY_0 , KEY_DOUBLE))
        {
            printf("Key_0 Double\n");
            LED_PWM = 200 ;
            Timer_Counter_Print();
            Wifi_Reconnect();
        }
        // 功能计时区
        Timer_Counter_Begin();

        LED_Write(LED_Status);
        PWM_Set_Duty_1024(LED_PWM , PWM_Channel_0);

        Timer_Counter_End();
        
        OLED_Update();
    }
}

// 定时器1ms中断
void Timer_Callback_1ms(void)
{
    static int tim_cnt = 0 ;
    tim_cnt ++ ;

    // 功能1: 1s
    if (tim_cnt >= 1000)
    {
        tim_cnt = 0 ;
    }

    // 功能2:检测按键状态1ms周期
    Key_Tick();
}
