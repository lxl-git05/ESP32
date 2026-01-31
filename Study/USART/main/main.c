#include "Initial.h"
#include "USART.h"

int check = 0 ;
char recData[128] ;

void app_main(void)
{
    Initial() ;
    USART_Init() ;
    
    while (1)
    {
        Timer_Counter_Func() ;
        //OLED展示参数
        OLED_Printf(0 , 0 , OLED_8X16 , "%d" , check++) ;
        // 计时
        Timer_Counter_Begin() ; // ============================ 计时开始

        ReceiveData_Zuse(recData) ;

        Timer_Counter_End();    // ============================ 计时结束

        // 按键
        if(Key_Check(KEY_0 , KEY_SINGLE))
        {
            printf("Key0 Single\n") ;
            sendData("Hello 666\n") ;
        }
        else if(Key_Check(KEY_0 , KEY_DOUBLE))
        {
            PWM_Set_Duty_1024(512 , PWM_Channel_0) ;
            Timer_Counter_Print() ;
        }
        OLED_Update();
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
