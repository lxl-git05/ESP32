#include "RGB_LED.h"
#include "driver/gpio.h"

void RGB_LED_Init(void)
{
    gpio_config_t LED_cfg = {

        .pin_bit_mask = 1ULL << RGB_LED_B_GPIO_NUM,     // GPIO口,需要使用掩码 1ULL << GPIO_NUM_x
        .mode = GPIO_MODE_OUTPUT,                 // GPIO模式
        .pull_up_en = GPIO_PULLUP_DISABLE,        // GPIO上下拉:失能
        .pull_down_en = GPIO_PULLDOWN_DISABLE,    // GPIO上下拉:失能
        .intr_type = GPIO_INTR_DISABLE,           // GPIO中断类型:失能

    };
    gpio_config(&LED_cfg);
}

void RGB_LED_Set(bool is_R_ON , bool is_G_ON ,bool is_B_ON)
{
    // gpio_set_level(RGB_LED_R_GPIO_NUM , is_R_ON) ;
    // gpio_set_level(RGB_LED_G_GPIO_NUM , is_G_ON) ;
    gpio_set_level(RGB_LED_B_GPIO_NUM , is_B_ON) ;
}
