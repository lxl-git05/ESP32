#ifndef __ADC_H
#define __ADC_H

#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ADC_GPIO_NUM 2  
#define ADC_Channel ADC_CHANNEL_1   // ADC_CHANNEL_1 GPIO2 - D1

#define ADC_Debug
extern int gray_data ;

// ADC数据初始化
void adc_init(void) ;

// raw读取
void  ADC_Read_Raw(int *ADC_Data) ;

// 任务:ADC灰度读取
void gray_task_tick(uint16_t vtaskdelay_ms) ;

#endif
