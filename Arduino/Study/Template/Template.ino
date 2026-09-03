// Template工程:模板工程

#include "Initial.h"

void setup() 
{
    Initial_All();
}


void loop() 
{
    // 监视函数采样约1秒，再等待2秒；调试控制台约每3秒输出一次。
    print_FreeRtos_Task();
    vTaskDelay(pdMS_TO_TICKS(2000));
    
}
