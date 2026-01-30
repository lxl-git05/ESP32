#include "Initial.h"

// WIFI模式下的需要外部声明的参数
char Command[64] = {0} ;
int LED_PWM = 0 ;
bool LED_Status ;
int led_state = 0;

void app_main(void)
{
    Initial() ;

    while (1)
    {
        Timer_Counter_Func() ;
        //OLED显示
        OLED_Printf(0 , 0 , OLED_8X16 , "%s" , "Hello World");
        // Key输入
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
        // LED
        LED_Write(led_state);
        PWM_Set_Duty_1024(LED_PWM , PWM_Channel_0);
        // 功能计时区
        Timer_Counter_Begin();

        

        Timer_Counter_End();
        
        OLED_Update();
    }
}

// 定时器1ms中断
void Timer_Callback_1ms(void)
{
    // 功能1:检测按键状态:1ms周期
    Key_Tick();
}
