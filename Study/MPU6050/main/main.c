#include <stdio.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "MyLED.h"
#include "OLED.h"
#include "Timer.h"
#include "Timer_Counter.h"
#include "MPU6050.h"
#include "MPU6050_Angle.h"

void app_main(void)
{
    OLED_Init();
    LED_Init();
    MPU6050_Init() ;                    // 初始化MPU6050
    MPU6050_Data_Error_Check(1000) ;    // 纠正零漂
    Timer_Init();
    while (1)
    {
        Timer_Counter_Func() ;
        //OLED展示参数
        OLED_ShowFloatNum( 0 ,  0 , MPU_Real.AccX , 3 , 3 , OLED_6X8 ) ;
        OLED_ShowFloatNum( 0 , 20 , MPU_Real.AccY , 3 , 3 , OLED_6X8 ) ;
        OLED_ShowFloatNum( 0 , 40 , MPU_Real.AccZ , 3 , 3 , OLED_6X8 ) ;

        OLED_ShowFloatNum(60 ,  0 , MPU_Real.pitch , 3 , 3 , OLED_6X8 ) ;
        OLED_ShowFloatNum(60 , 20 , MPU_Real.roll  , 3 , 3 , OLED_6X8 ) ;
        OLED_ShowFloatNum(60 , 40 , MPU_Real.yaw   , 3 , 3 , OLED_6X8 ) ;
        MPU6050_Update_Data() ;
        // MPU6050更新
        Timer_Counter_Begin() ; // ============================ 计时开始

        // 功能: 得到积分角度,但是时间不精确
        MPU6050_Raw_Error_Update();	// 更新去零参数
        static int MPU_Count  = 0 ;
        MPU_Count ++ ;
        if (MPU_Count == 10) // 10ms
        {
            MPU_Count = 0 ;
            MPU6050_Raw_Deal(10) ;	// 10ms更新
        }

        Timer_Counter_End();    // ============================ 计时结束

        printf("Timer Counter:%lld While Timer:%lld\n" , time_us , time_Func_us) ;

        // MPU6050静止检测 + 自动调节零漂
        MPU_Still_Check() ;
		MPU6050_Data_Error_Check_Auto() ;

        OLED_Update();
    }
}

// 定时器1ms中断
void Timer_Callback_1ms(void)
{
    static int tim_cnt = 0 ;
    tim_cnt ++ ;

    // 功能1: 1s 切换一次LED灯
    static int led_Status = 0 ;
    if (tim_cnt >= 1000)
    {
        led_Status = !led_Status ;
        tim_cnt = 0 ;
        LED_Write(led_Status);
    }
}
