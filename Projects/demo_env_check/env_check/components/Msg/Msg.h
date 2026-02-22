#ifndef __MSG_H
#define __MSG_H

#include <stdio.h>
#include "stdbool.h"
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ====================== 传感器数据 ======================
typedef struct 
{
    uint8_t temp ;      // 温度
    uint8_t humi ;      // 湿度
    float light ;       // 光强
    // 陀螺仪数据
	float	AccX ;	    // 加速度x 
	float	AccY ;	    // 加速度y
	float 	AccZ ;	    // 加速度z
	float	roll ;	    // 角度x 
	float	pitch;	    // 角度y
	float 	yaw  ;	    // 角度z
    // AI风险数据
    float risk_score ;
}Sensor_Data_Typedef;   // 传感器数据结构体

extern Sensor_Data_Typedef Sensor_Data ;
extern Sensor_Data_Typedef Sensor_Data_copy ;   // Sensor_Data的复制体,用于数据读取

// ================== 互斥锁数据 ==================

extern SemaphoreHandle_t data_Mutex ;    // 全局数据共享互斥锁
extern SemaphoreHandle_t I2C_Mutex ;    // IIC资源互斥锁

#endif
