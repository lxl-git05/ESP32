#include "Key_4.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Timer_Counter.h"

// #define Key_Debug
Key_Status_Typedef Key_S ;

// ====================== 外部任务 ======================

// 4按键传感器初始化
void Key_4_Init(void)
{
    adc_init(Key_Channel) ;
}

// 得到ADC的值
void Key_4_Get(int *Key_Data)
{
    ADC_Read_Raw(Key_Data , Key_Channel) ;
}

// 得到按键按下or没按下的状态
Key_Status_Typedef Key_4_Get_Status(void)
{
    Key_Status_Typedef temp = Key_S ;
    Key_S = Key_NULL ;
    return temp ;
}

// ADC检测任务
// =========== Task : 四按键模块数据读取(20ms) ============ 
void Task_Key4(void *param)
{
    Key_4_Init();

    int key_data = 0;
    Key_Status_Typedef Key_Status ;
    Key_Status_Typedef last_status = Key_NULL;   // 上一次状态
    uint8_t key_lock = 0;

    while (1)
    {
        Key_4_Get(&key_data);

        // ====== 判断当前按键 ======
        if (key_data > 3000)
        {
            Key_Status = KEY_4_Single;
        }
        else if (key_data > 2600 && key_data < 3000)
        {
            Key_Status = KEY_3_Single;
        }
        else if (key_data > 2300 && key_data < 2600)
        {
            Key_Status = KEY_2_Single;
        }
        else if (key_data > 2000 && key_data < 2250)
        {
            Key_Status = KEY_1_Single;
        }
        else
        {
            Key_Status = Key_NULL;
        }

        // ====== 边沿检测 ======
        

        if (Key_Status != Key_NULL && key_lock == 0)
        {
            key_lock = 1;
            printf("Key Pressed\n");
            Key_S = Key_Status ;
        }

        if (Key_Status == Key_NULL)
        {
            key_lock = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
