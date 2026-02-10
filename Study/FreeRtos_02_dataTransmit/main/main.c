#include "Initial.h"

void print_FreeRtos_Task(void) ;

void task1(void *param)
{
    // setup
    TickType_t pretime = xTaskGetTickCount() ;
    while (1)
    {
        LED_On() ;
        vTaskDelayUntil(&pretime , pdMS_TO_TICKS(500)) ;
        LED_OFF() ;
        vTaskDelayUntil(&pretime , pdMS_TO_TICKS(500)) ;
    }
}

void task2(void *param)
{
    // setup
    static int OLED_Num = 0 ;
    TickType_t pretime = xTaskGetTickCount() ;
    while (1)
    {
        OLED_ShowNum(0 , 0 , OLED_Num++ , 3 , OLED_8X16 ) ;
        OLED_Update() ;
        vTaskDelayUntil(&pretime , pdMS_TO_TICKS(1000)) ;
    }
    
}

void app_main(void)
{
    Initial() ;
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

void print_FreeRtos_Task(void)
{
    TaskStatus_t taskStats[12]; // 最多打印12个任务序列
    uint32_t totalRunTime;      // 运行时间
    // 得到task状态结构体
    UBaseType_t taskNum = uxTaskGetSystemState(taskStats,12,&totalRunTime);
    // 打表
    printf("Total Run Time: %lu\n", totalRunTime);
    printf("TaskNum: %d\n", taskNum);
    // 打印任务名字 , 优先级 , 运行状态 , 运行时间 , 最小剩余栈空间(单位为字)
    const char *state_str[] = { // 任务调度状态
        "Running", "Ready", "Blocked", "Suspended", "Deleted"
    }; 
    for (int i = 0; i < taskNum; i++)
    {
        float cpu_usage = (float)taskStats[i].ulRunTimeCounter * 100.0f / totalRunTime;
        printf("Task: %-12s | Pri: %d | State: ",
            taskStats[i].pcTaskName,
            taskStats[i].uxCurrentPriority);

        printf("%s", state_str[taskStats[i].eCurrentState]);

        printf(" | RunTime: %lu | CPU: %6.2f%% | StackFree: %lu\n",
            taskStats[i].ulRunTimeCounter,
            cpu_usage ,
            taskStats[i].usStackHighWaterMark);
    }
}