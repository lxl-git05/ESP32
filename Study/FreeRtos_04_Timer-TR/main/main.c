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

void task2(void *param)
{
    while (1)
    {
        
        vTaskDelay(pdMS_TO_TICKS(1000)) ;
    }
}

void app_main(void)
{
    Initial() ;
    // setup

    // 建立任务
    xTaskCreatePinnedToCore(task1 , "Task1" , 4096 , NULL , 1 , NULL , 0) ;
    xTaskCreatePinnedToCore(task2 , "Task2" , 4096 , NULL , 1 , NULL , 0) ;
    
    while (1)
    {
        // 按键
        if(Key_Check(KEY_0 , KEY_SINGLE))
        {
            printf("Key0 Single\n") ;
            print_FreeRtos_Task() ;
        }
        else if(Key_Check(KEY_0 , KEY_DOUBLE))
        {
            printf("Key0 Double\n") ;
            Timer_Counter_Print() ;
        }
        else if (Key_Check(KEY_0 , KEY_LONG))
        {
            
        }
        vTaskDelay(pdMS_TO_TICKS(10)) ;
    }
}

// 定时器1ms中断
void Timer_Callback_1ms(void)
{
    // 功能:Key更新
    Key_Tick() ;
}
