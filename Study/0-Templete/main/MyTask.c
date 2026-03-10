#include "MyTask.h"

// ====================== 必备函数 ======================
void main_Initial(void)
{
    // 一般外设初始化
    LED_Init();
    Key_Init();
    // 互斥锁设备初始化
    OLED_Init();
    // 定时器最后初始化
    Timer_Init();
}

void FreeRtos_Task_Print(void)
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

// ====================== 任务 ======================= 

// =========== Task: LED闪烁(1s亮1s暗) ============ 
void task_LED(void *param)
{
    while (1)
    {
        LED_On() ;
        vTaskDelay(pdMS_TO_TICKS(1000)) ;
        LED_OFF() ;
        vTaskDelay(pdMS_TO_TICKS(1000)) ;
    }
}

// ====================== 任务创建 ======================= 
void Task_Create(void)
{
    // 创建任务,RX属性在前,TX属性在后
    xTaskCreatePinnedToCore(task_LED , "task_LED" , 4096 , NULL , 1 , NULL , 0) ;
}
