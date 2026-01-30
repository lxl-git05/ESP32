#include <stdio.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "MyLED.h"
#include "OLED.h"
#include "Timer.h"
#include "Timer_Counter.h"
#include "MPU6050.h"

int check = 0 ;

void app_main(void)
{
    OLED_Init();
    LED_Init();
    Timer_Init();
    MPU6050_Init() ;    // 初始化MPU6050
    // MPU6050_Data_Error_Check() ;    // 纠正零漂
    while (1)
    {
        //OLED
        OLED_ShowChar(0, 0, check ++, OLED_8X16);
        
        // MPU6050更新
        Timer_Counter_Begin() ;

        MPU6050_Update_Data() ;

        Timer_Counter_End();
        printf("Timer Counter:%lld\n" , time_us) ;
        ESP_LOGI("main", "conut: %d", check);
        vTaskDelay(pdMS_TO_TICKS(1000));

        // MPU6050静止检测 + 自动调节零漂
        // MPU_Still_Check() ;
		// MPU6050_Data_Error_Check_Auto() ;

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

    // 功能2 : 得到积分角度
	// MPU6050_Raw_Error_Update();	// 更新去零参数
	// static int MPU_Count  = 0 ;
	// MPU_Count ++ ;
	// if (MPU_Count == 10) // 10ms
	// {
	// 	MPU_Count = 0 ;
	// 	MPU6050_Raw_Deal(10) ;	// 10ms更新
	// }
}
