#ifndef __MSG_H
#define __MSG_H

#include <stdio.h>
#include "stdbool.h"
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ====================== 传感器数据读写 ======================
typedef struct 
{
    // 检测数据
    float temp ;      // 温度
    float humi ;      // 湿度
    float Gray ;       // 光强
    // AI风险数据
    float risk ;
}Sensor_Data_Typedef;   // 传感器数据结构体

extern Sensor_Data_Typedef Sensor_Data ;
extern Sensor_Data_Typedef Sensor_Data_copy ;   // Sensor_Data的复制体,用于数据读取

// ================= 传感器数据存储(历史数据) =================
#define HISTORY_NUM 5

extern Sensor_Data_Typedef Sensor_History[HISTORY_NUM];
extern int history_head;   // 写入位置(0-HISTORY_NUM-1)

// ================== 事件组数据(4个数据都更新完成才进行历史数据更新) ==================
#define Temp_Humi_Bit 1 << 0
#define Gray_Bit      1 << 1
#define AI_Risk_Bit   1 << 2
extern EventGroupHandle_t sensorEventGroup;
void Task_History_Save(void* param) ;   // 历史事件更新任务

// ================== 互斥锁数据 ==================

extern SemaphoreHandle_t data_Mutex ;    // 全局数据读写共享互斥锁
extern SemaphoreHandle_t I2C_Mutex ;    // IIC资源互斥锁

#endif
