#ifndef __PWM_H
#define __PWM_H

#include "driver/ledc.h"
#include "driver/gpio.h"

#define PWM_Channel_0 LEDC_CHANNEL_0
#define PWM_Channel_1 LEDC_CHANNEL_1

void PWM_Init(void) ;

void PWM_Set_Duty_1024(int duty , int PWM_Channel) ;

#endif
