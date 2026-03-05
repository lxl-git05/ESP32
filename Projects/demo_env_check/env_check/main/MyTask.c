#include "MyTask.h"
// #include "ai_model.h"
uint8_t key_Status = 0 ;

// ====================== 必备函数 ======================
void main_Initial(void)
{
    // 一般外设初始化
    LED_Init();
    // RGB_LED_Init() ;
    // Buzzer_Init() ;
    // 互斥锁设备初始化
    OLED_Init();
    // MPU6050_Init() ; // 暂时不需要
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

// ====================== 外部任务 ======================= 

// =========== Task: LED闪烁(1000ms) ============ 
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

// =========== Task: OLED数据显示(100ms) ============ 
void Task_OLED(void *param)
{
    // setup
    // 存在互斥锁,所以在全局里面顺序进行初始化
    // loop
    while (1)
    {
        if (xSemaphoreTake(I2C_Mutex , portMAX_DELAY))
        {
            if (xSemaphoreTake(data_Mutex , portMAX_DELAY))
            {
                Sensor_Data_copy = Sensor_Data ;
                xSemaphoreGive(data_Mutex) ;
            }
            
            // OLED_Printf(0 ,  0 , OLED_6X8 , "Ax:%.2f  roll:%.2f" , 
            //            Sensor_Data_copy.AccX  , Sensor_Data_copy.roll) ;
            // OLED_Printf(0 , 20 , OLED_6X8 , "Ay:%.2f pitch:%.2f" ,
            //            Sensor_Data_copy.AccY  , Sensor_Data_copy.pitch) ;
            // OLED_Printf(0 , 40 , OLED_6X8 , "Az:%.2f   yaw:%.2f" , 
            //            Sensor_Data_copy.AccZ  , Sensor_Data_copy.yaw) ;
            static int num = 0 ;
            OLED_ShowNum(0 , 0 , num++ , 3 , OLED_8X16 ) ;

            // 尾处理
            OLED_Update() ; // 耗时:27ms
            xSemaphoreGive(I2C_Mutex) ;
        }
        vTaskDelay(pdMS_TO_TICKS(100)) ;
    }
}



// void test(void *param)
// {
//     ai_model_init();
//     printf(" ai_model_init() OK\n");
//     while (1)
//     {
//         risk_level = ai_predict_level(temp, humi, gray);
//         switch (risk_level)
//         {
//             case 0:
//                 printf("LOW\n");
//                 break;
//             case 1:
//                 printf("Mid\n");
//                 break;
//             case 2:
//                 printf("HIGH\n");
//                 break;
//             default:
//                 printf("No\n");
//                 break;
//         }
//         temp += 1 ;
//         humi -= 1 ;
//         printf("temp %f , humi %f , gray %f \n" , temp , humi , gray) ;
//         vTaskDelay(pdMS_TO_TICKS(1000)) ;
//     }
// }

// ====================== 函数调度控制器 ======================= 

void Task_Create(void)
{
    // 创建任务,RX属性在前,TX属性在后
    xTaskCreatePinnedToCore(task1 , "Task1" , 4096 , NULL , 1 , NULL , 0) ;
    xTaskCreatePinnedToCore(Task_OLED , "Task_OLED" , 4096 , NULL , 1 , NULL , 0) ;
    xTaskCreatePinnedToCore(Task_Key4 , "Task_Key4" , 4096 , NULL , 1 , NULL , 0) ;
    // 以后再记录振动
    // xTaskCreatePinnedToCore(Task_MPU  , "Task_MPU" , 4096 , NULL , 1 , NULL , 0) ;   
    xTaskCreatePinnedToCore(Task_DHT11, "Task_DHT11" , 3072 , NULL , 1 , NULL , 0) ;
    xTaskCreatePinnedToCore(Task_Gray , "Task_Gray" , 4096 , NULL , 1 , NULL , 0) ;

    // xTaskCreatePinnedToCore(test , "test" , 4096 , NULL , 1 , NULL , 0) ;
}
