#ifndef __PWM_H
#define __PWM_H

#include "driver/ledc.h"
#include "driver/gpio.h"

void PWM_Single_Init(int gpio_num , ledc_channel_t channel) ;

void PWM_Set_Duty_1024(int duty , int PWM_Channel) ;

#endif
