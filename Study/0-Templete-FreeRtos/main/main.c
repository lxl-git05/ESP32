#include "Initial.h"

void task1(void *param)
{
    while (1)
    {
        LED_On() ;
        vTaskDelay(pdMS_TO_TICKS(500)) ;
        LED_OFF() ;
        vTaskDelay(pdMS_TO_TICKS(500)) ;
    }
}

void app_main(void)
{
    Initial() ;
    // 创建通信道路,记得验证是否为NULL

    // 创建任务,RX属性在前,TX属性在后
    xTaskCreatePinnedToCore(task1 , "Task1" , 4096 , NULL , 1 , NULL , 0) ;
    // xTaskCreatePinnedToCore
    while (1)
    {
        // 按键
        if(Key_Check(KEY_0 , KEY_SINGLE))
        {
            printf("Key0 Single\n") ;
            print_FreeRtos_Task() ;         // 打印FreeRtos任务执行参数
        }
        else if(Key_Check(KEY_0 , KEY_DOUBLE))
        {
            printf("Key0 Double\n") ;
            Timer_Counter_Print() ;         // 打印计时参数
        }
        else if(Key_Check(KEY_0 , KEY_LONG))
        {
            printf("Key0 LONG\n") ;

        }
        vTaskDelay(pdMS_TO_TICKS(10)) ;
    }
}

// 定时器1ms中断,FreeRtos记得加ISR后缀
void Timer_Callback_1ms(void)
{
    // 功能1:Key更新
    Key_Tick() ;
}
