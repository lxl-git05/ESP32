#include "Msg.h"

// ================== 传感器数据 ==================
Sensor_Data_Typedef Sensor_Data ;
Sensor_Data_Typedef Sensor_Data_copy ;   // Sensor_Data的复制体,用于数据读取

// ================== 互斥锁数据 ==================
SemaphoreHandle_t data_Mutex = NULL ;    // 全局数据共享互斥锁
SemaphoreHandle_t I2C_Mutex  = NULL ;    // IIC资源互斥锁

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

// ==================  ==================