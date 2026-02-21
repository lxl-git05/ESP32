#include <stdio.h>
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "esp_attr.h"

#include "Timer.h"

// 供外部使用的中断弱定义回调函数:1ms调用一次
/*
    不准使用printf , ESP_LOGx , malloc , free , vtaskDelay 等函数
*/
__attribute__((weak))
void IRAM_ATTR Timer_Callback_1ms(void)
{
    
}


// 定义回调函数
bool gptimer_Callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    Timer_Callback_1ms();
    return true ;
}

// 定时器初始化
void Timer_Init(void)
{
    // 配置定时器
    gptimer_handle_t gptimer_handle ;
    static gptimer_config_t Timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT ,    // 默认时钟源
        .direction = GPTIMER_COUNT_UP ,         // 向上计数
        .resolution_hz = 1000000 ,              // 1M 分辨率
    };
    gptimer_new_timer( &Timer_config , &gptimer_handle) ;

    // 配置定时中断
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 1000,                 // 1k -> 1ms
        .reload_count = 0,                      // 重装载值
        .flags.auto_reload_on_alarm = true ,    // 自动重装载
    };
    gptimer_set_alarm_action(gptimer_handle, &alarm_config) ;

    // 配置中断回调函数
    gptimer_event_callbacks_t cbs = {
        .on_alarm = gptimer_Callback,
    };
    gptimer_register_event_callbacks(gptimer_handle , &cbs , NULL);

    // 使能定时器
    gptimer_enable(gptimer_handle);
    // 开启定时器
    gptimer_start(gptimer_handle);
}
