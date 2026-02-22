#include "Key_4.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Timer_Counter.h"

// #define Key_Debug
Key_Status_Typedef Key_Status ;

// ====================== 外部任务 ======================

// 4按键传感器
void Key_4_Init(void)
{
    adc_init(Key_Channel) ;
}

void Key_4_Get(int *Key_Data)
{
    ADC_Read_Raw(Key_Data , Key_Channel) ;
}

Key_Status_Typedef Key_4_Get_Status(void)
{
    return Key_Status ;
}

// =========== Task : 四按键模块数据读取(20ms) ============ 
void Task_Key4(void *param)
{
    // setup: ADC初始化
    Key_4_Init() ;
    int key_data = 0 ;
    while (1)
    {
        Key_4_Get(&key_data) ;
        if (key_data > 3000)
        {
            Key_Status = KEY_4_Single ;
        }
        else if (key_data > 2600 && key_data < 3000)
        {
            Key_Status = KEY_3_Single ;
        }
        else if (key_data > 2300 && key_data < 2600)
        {
            Key_Status = KEY_2_Single ;
        }
        else if (key_data > 2000 && key_data < 2250)
        {
            Key_Status = KEY_1_Single ;
        }
        else 
        {
            Key_Status = Key_NULL ;
        }     
        #ifdef Key_Debug 
        printf("key_status = %d\n" , Key_Status) ;
        #endif
        vTaskDelay(pdMS_TO_TICKS(20)) ;
    }
}

