#include "Buzzer.h"

#include <stdio.h>
#include "stdbool.h"
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Msg.h"
#include "PWM.h"

void Buzzer_Init(void)
{
    PWM_Single_Init(Buzzer_GPIO_NUM , Buzzer_Channel) ;
}

void Buzzer_Set_duty_1024(uint16_t duty)
{
    PWM_Set_Duty_1024(duty , Buzzer_Channel) ;
}
