#include "Initial.h"
void print_FreeRtos_Task(void) ;

#define Bits(x) (1 << (x))  // 位运算

SemaphoreHandle_t LED_Flag;
SemaphoreHandle_t Shared_Mutex ;
EventGroupHandle_t sensorGroup ;

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
    // setup
    while (1)
    {
        if (xSemaphoreTake(LED_Flag , portMAX_DELAY))
        {
            printf("LED Toggle\n") ;
        }
    }
}

void LowTask(void *param )
{
    printf("LOW TASK BUILD\n") ;
    while(1)
    {
        if (xSemaphoreTake(Shared_Mutex , portMAX_DELAY))
        {
            printf("LowPriorityTask: Holding shared resource  current priority: %u\n", uxTaskPriorityGet(NULL));
            vTaskDelay(pdMS_TO_TICKS(2000)); // Simulate long operation,高任务阻塞时没有优先继承,只有高任务就绪的时候才会给低优先级进行优先继承
            printf("LowPriorityTask: Releasing shared resource  current priority: %u\n", uxTaskPriorityGet(NULL));
            xSemaphoreGive(Shared_Mutex);   // 有take 就有 give 
        }
        vTaskDelay(pdMS_TO_TICKS(1000)) ;
    }

}

void MidTask(void *param)
{
    printf("MID TASK BUILD\n") ;
    while (1)
    {
        printf("Mid TASK:\n\n") ;
        printf("MediumPriorityTask: Doing background work\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void HighTask(void *param)
{
    printf("HIGH TASK BUILD\n") ;
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Let low task grab mutex first

        printf("HighPriorityTask: Needs shared resource\n");
        if (xSemaphoreTake(Shared_Mutex, portMAX_DELAY))
        {
            printf("HighPriorityTask: Got shared resource\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            printf("HighPriorityTask: Releasing shared resource\n");
            xSemaphoreGive(Shared_Mutex);
        }
    }
}

void Group1(void *param)
{
    while (1)
    {
        printf("Sensor1 Working\n") ;
        vTaskDelay(pdMS_TO_TICKS(2000)) ;
        printf("Sensor1 OK\n") ;
        xEventGroupSetBits(sensorGroup , Bits(0)) ;
        vTaskDelay(pdMS_TO_TICKS(2000)) ;
    }
}

void Group2(void *param)
{
    while (1)
    {
        printf("Sensor2 WOrking\n") ;
        vTaskDelay(pdMS_TO_TICKS(500)) ;
        printf("Sensor2 OK\n") ;
        xEventGroupSetBits(sensorGroup , Bits(1)) ;
        vTaskDelay(pdMS_TO_TICKS(2000)) ;
    }
    
}

void Group12(void *param)
{
    while (1)
    {
        printf("Processing Task: Waiting for both sensors...\n");
        xEventGroupWaitBits(sensorGroup , Bits(0) | Bits(1) , pdTRUE , pdTRUE , portMAX_DELAY) ;
        printf("Group1 and Group2 ALL OK , Begin TASK3 \n") ;
        vTaskDelay(pdMS_TO_TICKS(3000)) ;
        printf("ALL OK \n") ;
        vTaskDelay(pdMS_TO_TICKS(3000)) ;
    }
}


void app_main(void)
{
    Initial() ;
    // 建立信息
    LED_Flag = xSemaphoreCreateBinary() ;       // 二值量: app_main task2
    if (LED_Flag == NULL)
    {
        printf("Failed to create semaphore!\n") ;
        return ;
    }
    Shared_Mutex = xSemaphoreCreateMutex() ;    // 互斥锁: LowTask , MidTask , HighTask
    if (Shared_Mutex == NULL)
    {
        printf("Failed to create Mutex!\n") ;
        return ;
    }
    sensorGroup = xEventGroupCreate() ;
    if (sensorGroup == NULL)
    {
        printf("Failed to create Group!\n") ;
        return ;
    }
    // 建立任务
    xTaskCreatePinnedToCore(task1 , "Task1" , 4096 , NULL , 1 , NULL , 0) ; // LED灯

    // 二值信号任务
    xTaskCreatePinnedToCore(task2 , "Task2" , 4096 , NULL , 1 , NULL , 0) ;

    // 互斥锁任务
    // Priorities: High > Medium > Low
    // xTaskCreatePinnedToCore(LowTask, "LowPriority", 2048, NULL, 1, NULL, 1);
    // xTaskCreatePinnedToCore(MidTask, "MediumPriority", 2048, NULL, 2, NULL, 1);
    // xTaskCreatePinnedToCore(HighTask, "HighPriority", 2048, NULL, 3, NULL, 1);

    // 事件组任务
    // xTaskCreatePinnedToCore(Group1, "Group1"  , 2048, NULL, 2, NULL, 1);
    // xTaskCreatePinnedToCore(Group2, "Group2"  , 2048, NULL, 2, NULL, 1);
    // xTaskCreatePinnedToCore(Group12, "Group12", 2048, NULL, 2, NULL, 1);
    
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
            xSemaphoreGive(LED_Flag) ;
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
    // // 顺手打印队列状态
    // printf("Queue: used=%d free=%d\n",
    // uxQueueMessagesWaiting(Q_OLED_Data),
    // uxQueueSpacesAvailable(Q_OLED_Data));
}