#include "Msg.h"

// ================== 传感器数据 ==================
Sensor_Data_Typedef Sensor_Data ;
Sensor_Data_Typedef Sensor_Data_copy ;   // Sensor_Data的复制体,用于数据读取

// ================== 互斥锁数据 ==================
SemaphoreHandle_t data_Mutex = NULL ;    // 全局数据共享互斥锁
SemaphoreHandle_t I2C_Mutex  = NULL ;    // IIC资源互斥锁

// ================== 事件组数据 ==================
EventGroupHandle_t sensorEventGroup;

// ================== 历史数据 ==================
Sensor_Data_Typedef Sensor_History[HISTORY_NUM];
int history_head = 0;

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
    // 事件组的建立
    sensorEventGroup = xEventGroupCreate();
    if (sensorEventGroup == NULL)
    {
        printf("Failed to create Event!\n") ;
        return ;
    }
}

// ================== 历史数据更新 ==================
void Task_History_Save(void* param)
{
    while (1)
    {
        xEventGroupWaitBits(sensorEventGroup, Temp_Humi_Bit | Gray_Bit | AI_Risk_Bit,
                        pdTRUE, // Clear bits after waiting
                        pdTRUE, // Wait for both bits 
                        portMAX_DELAY);

        xSemaphoreTake(data_Mutex, portMAX_DELAY);

        Sensor_History[history_head] = Sensor_Data;

        history_head++;

        if(history_head >= HISTORY_NUM)
            history_head = 0;

        xSemaphoreGive(data_Mutex);

        printf("History Update: history_head = %d\n",history_head) ;

        vTaskDelay(pdMS_TO_TICKS(100)) ;
    }
}
