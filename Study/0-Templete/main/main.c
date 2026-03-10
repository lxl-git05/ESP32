#include "MyTask.h"

void app_main(void)
{
    main_Initial() ;
    // 创建通信道路,记得验证是否为NULL
    Msg_Create() ;
    // 创建任务,RX属性在前,TX属性在后
    Task_Create() ;

    while (1)
    {
        if (Key_Check(KEY_0 , KEY_SINGLE))
        {
            printf("Key_0 Single\n") ;
            FreeRtos_Task_Print() ;
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
