#include "Initial.h"

TaskHandle_t Task1Handle ;

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
        static int cnt = 0 ;
        printf("Task2 Run times : %d\n" , cnt ++ ); 
        if (cnt == 5)
        {
            vTaskDelete(NULL) ; // 自己删掉自己
        }
        vTaskDelay(pdMS_TO_TICKS(500)) ;
    }
}

void app_main(void)
{
    Initial() ;
    // 建立信号

    // 建立任务
    xTaskCreatePinnedToCore(task1 , "Task1" , 4096 , NULL , 1 , &Task1Handle , 0) ;
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
            static bool LED_Flag = true;
            if (LED_Flag)
            {
                printf("Task1 Suspend\n") ;
                vTaskSuspend(Task1Handle) ;
            }
            else
            {
                printf("Task1 Resumed\n") ;
                vTaskResume(Task1Handle) ;
            }
            LED_Flag = !LED_Flag ;

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
