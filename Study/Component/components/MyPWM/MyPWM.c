#include "MyPWM.h"
#include "driver/ledc.h"

void PWM_Init(void)
{
    // 初始化定时器
    ledc_timer_config_t ledc_timer = {
        .clk_cfg = LEDC_AUTO_CLK,               // 自动选择时钟源
        .speed_mode = LEDC_LOW_SPEED_MODE,      // 只有低速模式
        .timer_num = LEDC_TIMER_0,              // 0号定时器
        .duty_resolution = LEDC_TIMER_12_BIT,   // 占空比分辨率:2^12
        .freq_hz = 5000,                        // 频率5000Hz
    };
    // 初始化LED定时器
    ledc_timer_config(&ledc_timer);
    // 初始化PWM通道
    ledc_channel_config_t ledc_channel = {
        .channel = LEDC_CHANNEL_0,              // 通道
        .duty = 0,                              // 初始占空比
        .gpio_num = PWM_GPIO_NUM,               // GPIO引脚
        .speed_mode = LEDC_LOW_SPEED_MODE,      // 低速模式
        .timer_sel = LEDC_TIMER_0               // 使用0号定时器
    };
    // 初始化LED通道
    ledc_channel_config(&ledc_channel);
    // 渐变
    ledc_fade_func_install(0);
}