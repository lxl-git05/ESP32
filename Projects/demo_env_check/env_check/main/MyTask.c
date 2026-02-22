#include "MyTask.h"

// ====================== 全局变量 ======================
typedef struct 
{
    uint8_t temp ;      // 温度
    uint8_t humi ;      // 湿度
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
Sensor_Data_Typedef Sensor_Data_copy ;   // Sensor_Data的复制体,用于数据读取
uint8_t key_Status = 0 ;
SemaphoreHandle_t data_Mutex = NULL ;    // 全局数据共享互斥锁
SemaphoreHandle_t I2C_Mutex  = NULL ;    // IIC资源互斥锁

// ====================== 必备函数 ======================
void main_Initial(void)
{
    // 一般外设初始化
    LED_Init();

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

// =========== Task0: LED闪烁(1000ms) ============ 
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

// =========== Task1: DHT11温湿度读取(1000ms) ============ 
void Task_DHT11(void *param)
{
    // setup
    while(dht11_init())
	{
		printf("DHT11 Enable Error! Please Check Lines\r\n");
		vTaskDelay(pdMS_TO_TICKS(500));
	}
	printf("DHT11 Enable OK!\r\n");
    // 数据读取
    uint8_t temp = 0 ;
    uint8_t humi = 0 ;
    dht11_read_data(&temp, &humi);  // 第一次读取数据值不准确
    vTaskDelay(pdMS_TO_TICKS(10)) ;
    TickType_t lastWakeTime = xTaskGetTickCount();
    while (1) 
    {
        dht11_read_data(&temp, &humi);   /* 读取温湿度值 */
        if (xSemaphoreTake(data_Mutex , portMAX_DELAY))
        {
            Sensor_Data.temp = temp ;
            Sensor_Data.humi = humi ;
            xSemaphoreGive(data_Mutex) ;
        }
        #ifdef DHT_Debug
        printf("\ntemp:%d degree\n",temp);
        printf("humi:%d%%\n",humi);
        #endif
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000));
    }  
}

// =========== Task2: 灰度传感器数据读取(1000ms) ============ 
void Task_Gray(void *param)
{
    // setup
    ADC_Gray_Init() ;
    TickType_t lastWakeTime = xTaskGetTickCount();
    int dht_data = 0 ;
    // loop
    while (1) // 500us完成一次
    {
        // 数据读取
        ADC_Get_Gray(&dht_data) ;
        // 数据写入
        if (xSemaphoreTake(data_Mutex , portMAX_DELAY))
        {
            Sensor_Data.light = dht_data * 1.0 / 4095 ; // 灰度:0-1
            #ifdef ADC_Debug
            printf("Gray Data = %.2f , int = %d \n" , Sensor_Data.light , dht_data) ;
            #endif
            xSemaphoreGive(data_Mutex) ;
        }
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000));
    }
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
            if (xSemaphoreTake(data_Mutex , portMAX_DELAY))
            {
                Sensor_Data_copy = Sensor_Data ;
                xSemaphoreGive(data_Mutex) ;
            }
            
            OLED_Printf(0 ,  0 , OLED_6X8 , "Ax:%.2f  roll:%.2f" , 
                       Sensor_Data_copy.AccX  , Sensor_Data_copy.roll) ;
            OLED_Printf(0 , 20 , OLED_6X8 , "Ay:%.2f pitch:%.2f" ,
                       Sensor_Data_copy.AccY  , Sensor_Data_copy.pitch) ;
            OLED_Printf(0 , 40 , OLED_6X8 , "Az:%.2f   yaw:%.2f" , 
                       Sensor_Data_copy.AccZ  , Sensor_Data_copy.yaw) ;

            // 尾处理
            OLED_Update() ; // 耗时:27ms
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
    MPU_Offset.AccErrorX  = 0.0927990749f ;
    MPU_Offset.AccErrorY  = -0.0469766855f;
    MPU_Offset.AccErrorZ  = 0.00974702835f;
    MPU_Offset.GyroErrorX = -2.07370448f  ;
    MPU_Offset.GyroErrorY = 1.26626182f   ;
    MPU_Offset.GyroErrorZ = 0.0725877061f ;
    #define MPU_Delay_time_ms 25    // MPU6050采样时间(ms)
    // loop
    while (1)
    {
        Timer_Counter_Func() ;
        if (xSemaphoreTake(I2C_Mutex , portMAX_DELAY))
        {
            // 三项总共550us
            MPU6050_Update_Data() ; // 470 us

            MPU6050_Raw_Error_Update() ;

            MPU6050_Raw_Deal(MPU_Delay_time_ms) ;
            if (xSemaphoreTake(data_Mutex , portMAX_DELAY))
            {
                Sensor_Data.AccX = MPU_Real.AccX ;
                Sensor_Data.AccY = MPU_Real.AccY ;
                Sensor_Data.AccZ = MPU_Real.AccZ ;
                Sensor_Data.roll = MPU_Real.roll ;
                Sensor_Data.pitch= MPU_Real.pitch;
                Sensor_Data.yaw  = MPU_Real.yaw  ;
                xSemaphoreGive(data_Mutex) ;
            }
            // 尾处理
            xSemaphoreGive(I2C_Mutex) ;
        }
        vTaskDelay(pdMS_TO_TICKS(MPU_Delay_time_ms)) ;
    } 
}

// =========== Task5: 四按键模块数据读取 ============ 
void Task_Key4(void *param)
{
    // setup: ADC初始化
    ADC_Key_Init() ;
    int key_data = 0 ;
    while (1)
    {
        Timer_Counter_Begin() ;
        ADC_Get_Key(&key_data) ;
        Timer_Counter_End() ;
        if (key_data > 3000)
        {
            key_Status = 4 ;
        }
        else if (key_data > 2600 && key_data < 3000)
        {
            key_Status = 3 ;
        }
        else if (key_data > 2300 && key_data < 2600)
        {
            key_Status = 2 ;
        }
        else if (key_data > 2000 && key_data < 2250)
        {
            key_Status = 1 ;
        }
        else 
        {
            key_Status = 0 ;
        }     
        printf("key_status = %d\n" , key_Status) ;
        vTaskDelay(pdMS_TO_TICKS(20)) ;
    }
}

// ====================== 函数调度控制器 ======================= 

void Task_Create(void)
{
    // 创建任务,RX属性在前,TX属性在后
    xTaskCreatePinnedToCore(task1 , "Task1" , 4096 , NULL , 1 , NULL , 0) ;
    xTaskCreatePinnedToCore(Task_OLED , "Task_OLED" , 4096 , NULL , 1 , NULL , 0) ;
    xTaskCreatePinnedToCore(Task_Key4 , "Task_Key4" , 4096 , NULL , 1 , NULL , 0) ;
    xTaskCreatePinnedToCore(Task_MPU  , "Task_MPU" , 4096 , NULL , 1 , NULL , 0) ;
    xTaskCreatePinnedToCore(Task_DHT11, "Task_DHT11" , 3072 , NULL , 1 , NULL , 0) ;
    xTaskCreatePinnedToCore(Task_Gray , "Task_Gray" , 2048 , NULL , 1 , NULL , 0) ;
}
