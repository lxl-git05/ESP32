#include "PWM.h"

#define PWM_GPIO_PIN_0  GPIO_NUM_5
#define PWM_GPIO_PIN_1  GPIO_NUM_6

void PWM_Single_Init(int gpio_num , ledc_channel_t channel)
{
    // 定时器TIM初始化
    ledc_timer_config_t Timer_PWM_cfg = {
        .timer_num = LEDC_TIMER_0 ,                 // 定时器0
        .clk_cfg = LEDC_AUTO_CLK,                   // 自动选择
        .duty_resolution = LEDC_TIMER_10_BIT ,      // 1/1024的分辨率
        .freq_hz = 5000 ,                           // 5KHZ频率
        .speed_mode = LEDC_LOW_SPEED_MODE ,         // 推荐LOW_SPEED_MODE
    };

    ledc_timer_config(&Timer_PWM_cfg);

    // 通道初始化

    ledc_channel_config_t channel_cfg = {
        .gpio_num = gpio_num ,                // GPIO
        .channel = channel ,                  // Channel
        .duty = 0 ,
        .hpoint = 0 ,
        .intr_type = LEDC_INTR_DISABLE ,
        .speed_mode = LEDC_LOW_SPEED_MODE ,
        .timer_sel = LEDC_TIMER_0 ,
    };
    ledc_channel_config(&channel_cfg) ;
}

void PWM_Init(void)
{
    // fade初始化,使得PWM变化平滑
    ledc_fade_func_install(0);  // 0 表示不分配专用内存
    // PWM初始化
    PWM_Single_Init(PWM_GPIO_PIN_0 , PWM_Channel_0);
    PWM_Single_Init(PWM_GPIO_PIN_1 , PWM_Channel_1);
}

void PWM_Set_Duty_1024(int duty , int PWM_Channel)
{
    if (duty < 0)
    {
        duty = 0 ;
    }
    else if (duty > 1024)
    {
        duty = 1024 ;
    }
    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, PWM_Channel, duty, 0);
}
