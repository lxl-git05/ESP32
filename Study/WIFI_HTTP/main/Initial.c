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