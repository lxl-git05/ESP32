#include <stdio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include "print.h"

// 产生红线使用ctrl + shift + p 添加vscode配置文件夹即可

void app_main(void)
{
    Print_Hello() ;
    // 前半部分是tag,提示作用,后半部分是语句
    while (1)
    {
        ESP_LOGI("main" , "hello world") ;  // 自带换行
        printf("hello_printf");             // 直接打印，没有tag显示,并且需要自己输入换行
        vTaskDelay(pdMS_TO_TICKS(1000));   // 参数是ticks,pdMS_TO_TICKS可以ms转ticks
    }
}
