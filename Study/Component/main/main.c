#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "MyLED.h"
#include "driver/ledc.h"

#include "MyPWM.h"

void app_main(void)
{
    led_init();
    int led_level = 0 ;

    PWM_Init();

    while (1)
    {
        // GPIO¿ØÖÆLED
        gpio_set_level(LED_GPIO_NUM, led_level);

        led_level = !led_level;

        printf("led_level: %d\n", led_level);

        vTaskDelay(pdMS_TO_TICKS(1000));
        // PWM
        ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 4095, 2000);
        ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE);
        ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0, 2000);
        ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE);
    }
    

}
