#include "Initial.h"

void Initial(void)
{
    // 一般外设初始化
    OLED_Init();
    LED_Init();
    Key_Init();
    PWM_Init();
    // 外设初始化
    // MPU6050_Init() ;                    // 初始化MPU6050
    // MPU6050_Data_Error_Check(1000) ;    // 纠正零漂,1000次采样

    // 定时器最后初始化
    Timer_Init();
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
    // 顺手打印队列状态
    // printf("Queue: used=%d free=%d\n",
    // uxQueueMessagesWaiting(Q_OLED_Data),
    // uxQueueSpacesAvailable(Q_OLED_Data));
}
