#include "MyTask.h"

// ====================== 必备函数 ======================
void main_Initial(void)
{
    // 一般外设初始化
    LED_Init();
    // 互斥锁设备初始化
    OLED_Init();
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
}

// ====================== 外部任务 ======================= 

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

// =========== Task: AI模型风险预测 ============ 
void Task_AI_Predict(void*param)
{
    // AI模型初始化
    Ai_Init() ;
    
    // AI预测模型
    while (1)
    {   
        float risk = Ai_Predict(Sensor_History[history_head].temp, 
        Sensor_History[history_head].humi, Sensor_History[history_head].Gray);

        if (xSemaphoreTake(data_Mutex , portMAX_DELAY))
        {
            // 本次数据的结果表明DHT11还在初始化中,所以risk清零
            if (Sensor_History[history_head].temp < 0.1f && Sensor_History[history_head].humi < 0.1f)
            {
                risk = 0.0f ;
            }
            Sensor_Data.risk = risk ;
            xSemaphoreGive(data_Mutex) ;
        }
        
        xEventGroupSetBits(sensorEventGroup, AI_Risk_Bit);  // AI预测任务完成标志位

        vTaskDelay(pdMS_TO_TICKS(1000)) ;
    }
}

void Task_RGY_Buzzer(void*param)
{
    RGB_LED_Init() ;
    Buzzer_Init() ;
    static float risk_current ;
    while (1)
    {
        // 数据读取
        static float risk_last = 0.0f ;
        if (xSemaphoreTake(data_Mutex , portMAX_DELAY))
        {
            risk_current = Sensor_Data.risk ;
            xSemaphoreGive(data_Mutex) ;
        }
    
        // 一般情况下是绿色
        if (risk_current < 1.0f && risk_current > 0)
        {
            RGB_LED_Set(false,true,false) ; // 绿色:安全
            Buzzer_Set_duty_1024(0) ;       // 提示结束,停止鸣叫
        }
        // 预测风险从正常转为可能存在风险时,蜂鸣器短鸣3次进行提示
        else if (risk_last < 1.0f && (risk_current >= 1.0f && risk_current < 2.0f))
        {
            Buzzer_Set_duty_1024(50) ;
            vTaskDelay(pdMS_TO_TICKS(100)) ;
            Buzzer_Set_duty_1024(0) ;
            vTaskDelay(pdMS_TO_TICKS(100)) ;

            Buzzer_Set_duty_1024(50) ;
            vTaskDelay(pdMS_TO_TICKS(100)) ;
            Buzzer_Set_duty_1024(0) ;
            vTaskDelay(pdMS_TO_TICKS(100)) ;

            Buzzer_Set_duty_1024(50) ;
            vTaskDelay(pdMS_TO_TICKS(100)) ;
            Buzzer_Set_duty_1024(0) ;   // 提示结束,停止鸣叫

            RGB_LED_Set(false,false,true) ; // 绿色 -> 黄色
        }
        // 预测风险达到阈值时蜂鸣器长鸣警报,三色LED转为红色
        else if (risk_current >= 2.0f)
        {
            Buzzer_Set_duty_1024(200) ; // 长鸣

            RGB_LED_Set(true , false , false) ; // -> 红色
        }

        risk_last = risk_current ;  // 更新risk

        vTaskDelay(pdMS_TO_TICKS(100)) ;  
    }
}

// ====================== 任务创建 ======================= 
void Task_Create(void)
{
    
    // 创建任务,RX属性在前,TX属性在后
    xTaskCreatePinnedToCore(task_LED , "task_LED" , 4096 , NULL , 1 , NULL , 0) ;
    xTaskCreatePinnedToCore(Task_Key4 , "Task_Key4" , 4096 , NULL , 1 , NULL , 0) ; 
    xTaskCreatePinnedToCore(Task_Menu , "Task_Menu" , 4096 , NULL , 1 , NULL , 0) ;

    xTaskCreatePinnedToCore(Task_DHT11, "Task_DHT11" , 3072 , NULL , 1 , NULL , 1) ;
    xTaskCreatePinnedToCore(Task_Gray , "Task_Gray" , 4096 , NULL , 1 , NULL , 1) ;
    xTaskCreatePinnedToCore(Task_AI_Predict , "Task_AI_Predict" , 4096 , NULL , 1 , NULL , 1) ;
    xTaskCreatePinnedToCore(Task_RGY_Buzzer , "Task_RGY_Buzzer" , 4096 , NULL , 1 , NULL , 1) ;

    xTaskCreatePinnedToCore(Task_History_Save , "Task_H_Save" , 4096 , NULL , 1 , NULL , 0) ;
}
