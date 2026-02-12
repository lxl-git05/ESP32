#include "Initial.h"

TimerHandle_t Timer1 ;

TaskHandle_t Task1Handle;
TaskHandle_t Task2Handle;
TaskHandle_t Task3Handle;

void task1(void *param)
{
    while (1)
    {
        printf("Task1: Sending notification to Task2\n");
        xTaskNotifyGive(Task2Handle);
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
        ulTaskNotifyTake(pdTRUE , portMAX_DELAY) ;
        printf("Task2 : Get Msg From Task1 \n") ;
        vTaskDelay(pdMS_TO_TICKS(1000)) ;
    }
}

void task3(void *param)
{
    static uint32_t RxData = 0 ;
    while (1)
    {
        xTaskNotifyWait(0x00 , 0x00 , &RxData , portMAX_DELAY) ;
        printf("Task3 Get Data From ISR: %lu \n" , RxData ) ;
    }
}

void Timer1_cb(TimerHandle_t xTimer) 
{
    Timer_Counter_Func() ;
    printf("Timer_Callback\n") ;
}

void app_main(void)
{
    Initial() ;
    // setup

    // 建立任务:接收任务通信需要放在发送前面
    xTaskCreatePinnedToCore(task2 , "Task2" , 4096 , NULL , 1 , &Task2Handle , 0) ;
    xTaskCreatePinnedToCore(task3 , "Task3" , 2048 , NULL , 1 , &Task3Handle , 0) ;
    xTaskCreatePinnedToCore(task1 , "Task1" , 4096 , NULL , 1 , &Task1Handle , 0) ;
    // 建立中断
    Timer1 = xTimerCreate("Timer1" , pdMS_TO_TICKS(1000) , pdTRUE , 0 , Timer1_cb ) ;
    if (Timer1 == NULL)
    {
        printf("Timer creation failed!\n");
    }
    else
    {
        printf("Starting timer...\n");
        xTimerStart(Timer1, 0);
    }
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
    // 功能:1s发送一次数据给Task3
    static int cnt = 1 ;
    static int TxData = 0x00 ;
    cnt ++ ;
    if (cnt >= 1000 )
    {
        cnt = 1 ;
        xTaskNotifyFromISR(Task3Handle , TxData++ , eSetValueWithOverwrite , NULL) ;
    }
}
