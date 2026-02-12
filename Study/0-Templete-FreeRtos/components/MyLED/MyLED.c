#include "MyLED.h"
#include "driver/gpio.h"

void LED_Init(void)
{
    gpio_config_t LED_cfg = {

        .pin_bit_mask = 1ULL << LED_GPIO_NUM,     // GPIO口,需要使用掩码 1ULL << GPIO_NUM_x

        .mode = GPIO_MODE_OUTPUT,               // GPIO模式

        .pull_up_en = GPIO_PULLUP_DISABLE,      // GPIO上下拉:失能

        .pull_down_en = GPIO_PULLDOWN_DISABLE,  // GPIO上下拉:失能

        .intr_type = GPIO_INTR_DISABLE,         // GPIO中断类型:失能

    };

    gpio_config(&LED_cfg);
}

void LED_Write(int level)
{
    gpio_set_level(LED_GPIO_NUM, level);
}

void LED_On(void)
{
    gpio_set_level(LED_GPIO_NUM, 1);
}

void LED_OFF(void)
{
    gpio_set_level(LED_GPIO_NUM, 0);
}
