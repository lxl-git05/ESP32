#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "OLED.h"

int check = 0 ;

void app_main(void)
{
    OLED_Init();

    while (1)
    {
        //Õý³£ÏÔÊ¾
        OLED_Clear();
        OLED_ShowChar(0, 0, check ++, OLED_8X16);
        OLED_ShowString(0, 16, "Hello World!", OLED_6X8);
        OLED_ShowNum(0, 32, 123456, 6, OLED_8X16);
        OLED_ShowSignedNum(0, 48, -123456, 6, OLED_8X16);
        OLED_Update();

        printf("%d\n",check);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
