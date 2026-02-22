#ifndef __BUZZER_H
#define __BUZZER_H

#include "driver/gpio.h"

#define Buzzer_GPIO_NUM  GPIO_NUM_9         // D10
#define Buzzer_Channel   LEDC_CHANNEL_0

// ≥ı ºªØ
void Buzzer_Init(void);
// …Ë÷√PWM
void Buzzer_Set_duty_1024(uint16_t duty) ;

#endif
