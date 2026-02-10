#include "Initial.h"

int check = 0 ;

void print_FreeRtos_Task(void) ;

void task1(void *pvparam)
{
    // 只执行一遍,后续的"上次执行时间"vTaskDelayUntil会自己更新
    TickType_t lastWakeTime = xTaskGetTickCount();
    while (1)
    {
        LED_On() ;
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(500));
        LED_OFF() ;
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(500));
    }
}

void task2(void *pvparam)
{
    static int PWM = 0 ;
    int add = 1 ;
    while (1)
    {
        PWM += add ;
        PWM_Set_Duty_1024(PWM, PWM_Channel_0) ;
        if (PWM <= 0 || PWM >= 1024)
        {
            add = -add ;
        }
        vTaskDelay(pdMS_TO_TICKS(500)) ;
    }
}

void app_main(void)
{
    Initial() ;

    xTaskCreate(task1 , "task1_Blink" , 2048 , NULL , 2 , NULL ) ;
    xTaskCreate(task2 , "task2_PWM"   , 2048 , NULL , 1 , NULL ) ;
    
    while (1)
    {
        Timer_Counter_Func() ;
        //OLED展示参数
        OLED_Printf(0 , 0 , OLED_8X16 , "%d" , check++) ;
        // 计时
        Timer_Counter_Begin() ; // ============================ 计时开始


        Timer_Counter_End();    // ============================ 计时结束

        // 按键
        if(Key_Check(KEY_0 , KEY_SINGLE))
        {
            print_FreeRtos_Task() ;
            printf("Key0 Single\n") ;
            PWM_Set_Duty_1024(0 , PWM_Channel_0) ;
        }
        else if(Key_Check(KEY_0 , KEY_DOUBLE))
        {
            PWM_Set_Duty_1024(512 , PWM_Channel_0) ;
            Timer_Counter_Print() ;
        }
        OLED_Update();
        vTaskDelay(pdMS_TO_TICKS(1)) ;
    }
}

// 定时器1ms中断
void Timer_Callback_1ms(void)
{
    static int tim_cnt = 0 ;
    tim_cnt ++ ;
    // 功能1:Key更新
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
