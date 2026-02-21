#include "Initial.h"

void task1(void *param)
{
    while (1)
    {
        LED_On() ;
        vTaskDelay(pdMS_TO_TICKS(500)) ;
        LED_OFF() ;
        vTaskDelay(pdMS_TO_TICKS(500)) ;
    }
}

void Task_DHT11(void *param)
{
    // 1000ms执行一次
    dht11_task_tick(1000) ;
}

void Task_ADC(void *param)
{
    // 1000ms执行一次
    gray_task_tick(1000) ;
}

void app_main(void)
{
    Initial() ;
    // 创建通信道路,记得验证是否为NULL

    // 创建任务,RX属性在前,TX属性在后
    xTaskCreatePinnedToCore(task1 , "Task1" , 4096 , NULL , 1 , NULL , 0) ;
    // xTaskCreatePinnedToCore(Task_DHT11 , "Task_DHT11" , 3072 , NULL , 1 , NULL , 0) ;
    xTaskCreatePinnedToCore(Task_ADC , "Task_ADC" , 2048 , NULL , 1 , NULL , 0) ;

    while (1)
    {
        // 按键
        if(Key_Check(KEY_0 , KEY_SINGLE))
        {
            printf("Key0 Single\n") ;
            print_FreeRtos_Task() ;         // 打印FreeRtos任务执行参数
        }
        else if(Key_Check(KEY_0 , KEY_DOUBLE))
        {
            printf("Key0 Double\n") ;
            Timer_Counter_Print() ;         // 打印计时参数
        }
        else if(Key_Check(KEY_0 , KEY_LONG))
        {
            printf("Key0 LONG\n") ;
            
        }
        vTaskDelay(pdMS_TO_TICKS(10)) ;
    }
}

// 定时器1ms中断,FreeRtos记得加ISR后缀
void Timer_Callback_1ms(void)
{
    // 功能1:Key更新
    Key_Tick() ;
}


/*
任务要求:
    1. 采集温度、湿度、环境光强、震动或倾斜等多源数据，并间隔一段时间持续记录。
    2. 使用TensorFlow等平台训练轻量模型，输出风险预测。
    3. OLED上设置多级菜单，显示各传感器实时值、预测评分与趋势、过往采集数据等，使用四按键控制菜单切换、数据显示、数据情况、重置设备等；三色LED用红黄绿显示风险等级。
    4. 预测风险从正常转为可能存在风险时，蜂鸣器短鸣3次进行提示，三色LED由绿色转为黄色；预测风险达到阈值时蜂鸣器长鸣警报，三色LED转为红色。
*/