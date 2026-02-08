#include "Initial.h"
#include "Wifi_Manager.h"
#include "CAM.h"

float hum = 0.0f ;
float temp = 26.0f ;

void app_main(void)
{
    Initial() ;
    // Wifi_Init() ;   // 初始化WIFI连接,并且内含HTTP协议过程
    Cam_Init() ;
    
    while (1)
    {
        Timer_Counter_Func() ;
        
        // 计时
        Timer_Counter_Begin() ; // ============================ 计时开始

        picture() ;

        Timer_Counter_End();    // ============================ 计时结束

        // 按键
        if(Key_Check(KEY_0 , KEY_SINGLE))
        {
            printf("Key0 Single\n") ;
            hum += 0.105 ;
            PWM_Set_Duty_1024(  0 , PWM_Channel_0) ;
        }
        else if(Key_Check(KEY_0 , KEY_DOUBLE))
        {
            temp += 2.456 ;
            PWM_Set_Duty_1024(512 , PWM_Channel_0) ;
            Timer_Counter_Print() ;
        }
        else if (Key_Check(KEY_0 , KEY_LONG))
        {
            // WIFI断连十次之后就不再尝试重连,此时只能Reset或者长按按键手动重连
            Wifi_Reconnect() ;
        }
        // OLED_Update();
    }
}

// 定时器1ms中断
void Timer_Callback_1ms(void)
{
    static int tim_cnt = 0 ;
    tim_cnt ++ ;

    // 功能1:Key更新
    Key_Tick() ;

    // 功能2: 1s 切换一次LED灯
    static int led_Status = 0 ;
    if (tim_cnt >= 1000)
    {
        led_Status = !led_Status ;
        tim_cnt = 0 ;
        LED_Write(led_Status);
    }
}
