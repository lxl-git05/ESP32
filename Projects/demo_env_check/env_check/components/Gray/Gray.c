#include "Gray.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Msg.h"

#define Gray_Debug

// 灰度传感器初始化
void Gray_Init(void)
{
    adc_init(Gray_Channel) ;
}

// 灰度传感器数据读取
void Gray_Get(int *Gray_Data)
{
    ADC_Read_Raw(Gray_Data , Gray_Channel) ;
}

// =========== Task: 灰度传感器数据读取(1000ms) ============ 
void Task_Gray(void *param)
{
    // setup
    Gray_Init() ;
    TickType_t lastWakeTime = xTaskGetTickCount();
    int dht_data = 0 ;
    // loop
    while (1) // 500us完成一次
    {
        // 数据读取
        Gray_Get(&dht_data) ;
        // 数据写入
        if (xSemaphoreTake(data_Mutex , portMAX_DELAY))
        {
            Sensor_Data.light = dht_data * 1.0 / 4095 ; // 灰度:0-1
            #ifdef Gray_Debug
            printf("Gray Data = %.2f , int = %d \n" , Sensor_Data.light , dht_data) ;
            #endif
            xSemaphoreGive(data_Mutex) ;
        }
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000));
    }
}
