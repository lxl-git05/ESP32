#include "MyTask.h"

// ====================== 全局变量 ======================
typedef struct 
{
    int temp ;      // 温度
    int humi ;      // 湿度
    float light ;   // 光强
    // 陀螺仪数据
	float	AccX ;	// 加速度x 
	float	AccY ;	// 加速度y
	float 	AccZ ;	// 加速度z
	float	roll ;	// 角度x 
	float	pitch ;	// 角度y
	float 	yaw ;	// 角度z
    // AI风险数据
    float risk_score ;
}Sensor_Data_Typedef;

Sensor_Data_Typedef Sensor_Data ;
SemaphoreHandle_t data_Mutex = NULL ;    // 全局数据共享互斥锁
SemaphoreHandle_t I2C_Mutex  = NULL ;    // IIC资源互斥锁

// ====================== 必备函数 ======================
void main_Initial(void)
{
    // 一般外设初始化
    LED_Init();
    Key_Init();

    // 互斥锁设备初始化
    OLED_Init();
    MPU6050_Init() ;

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

void Msg_Create(void)
{
    // 互斥锁的建立
    data_Mutex = xSemaphoreCreateMutex() ;
    I2C_Mutex  = xSemaphoreCreateMutex() ;
    if (data_Mutex == NULL || I2C_Mutex == NULL)
    {
        printf("Failed to create Mutex!\n") ;
        return ;
    }
}

// ====================== 外部任务 ======================= 

// =========== Task0: LED闪烁 ============ 
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

// =========== Task1: DHT11温湿度读取 ============ 
void Task_DHT11(void *param)
{
    // 1000ms执行一次
    dht11_task_tick(1000) ;
}

// =========== Task2: 灰度传感器数据读取 ============ 
void Task_ADC(void *param)
{
    // 1000ms执行一次
    gray_task_tick(1000) ;
}

// =========== Task3: OLED数据显示 ============ 
void Task_OLED(void *param)
{
    // setup
    // 存在互斥锁,所以在全局里面顺序进行初始化
    // loop
    while (1)
    {
        if (xSemaphoreTake(I2C_Mutex , portMAX_DELAY))
        {
            static int oled_check = 0 ;
            OLED_ShowNum(0 , 0 , oled_check++ , 6 , OLED_8X16);
            OLED_Update() ; // 耗时:27ms

            // 尾处理
            xSemaphoreGive(I2C_Mutex) ;
        }
        vTaskDelay(pdMS_TO_TICKS(100)) ;
    }
}

// =========== Task4: MPU6050数据读取 ============ 
void Task_MPU(void *param)
{
    // setup
    // 存在互斥锁,所以在全局里面顺序进行初始化
    // loop
    while (1)
    {
        if (xSemaphoreTake(I2C_Mutex , portMAX_DELAY))
        {
            Timer_Counter_Func() ;
            Timer_Counter_Begin() ;

            MPU6050_Update_Data() ; // 470 us

            Timer_Counter_End() ;

            // printf("MPU data :\nAx\tAy\tAz\n %.2f\t%.2f\t%.2f\nGx\tGy\tGz\n %.2f\t%.2f\t%.2f\n" , 
            //       MPU_Raw_Data.AX , MPU_Raw_Data.AY , MPU_Raw_Data.AZ , 
            //       MPU_Raw_Data.GX , MPU_Raw_Data.GY , MPU_Raw_Data.GZ) ;
    
            // 尾处理
            xSemaphoreGive(I2C_Mutex) ;
        }
        vTaskDelay(pdMS_TO_TICKS(20)) ;
    } 
}

// ====================== 函数调度控制器 ======================= 

void Task_Create(void)
{
    // 创建任务,RX属性在前,TX属性在后
    xTaskCreatePinnedToCore(task1 , "Task1" , 4096 , NULL , 1 , NULL , 0) ;
    xTaskCreatePinnedToCore(Task_OLED , "Task_OLED" , 4096 , NULL , 1 , NULL , 0) ;
    // xTaskCreatePinnedToCore(Task_MPU , "Task_MPU" , 4096 , NULL , 1 , NULL , 0) ;
    // xTaskCreatePinnedToCore(Task_DHT11 , "Task_DHT11" , 3072 , NULL , 1 , NULL , 0) ;
    // xTaskCreatePinnedToCore(Task_ADC , "Task_ADC" , 2048 , NULL , 1 , NULL , 0) ;
}
