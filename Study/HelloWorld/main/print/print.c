#include "print.h"
#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include "freertos/task.h"

void Print_Hello(void)
{
    for (int i = 0 ; i < 10 ; i ++)
    {
        ESP_LOGI("log" , "Hello") ;
        vTaskDelay(pdMS_TO_TICKS(1000)) ;
    }
}