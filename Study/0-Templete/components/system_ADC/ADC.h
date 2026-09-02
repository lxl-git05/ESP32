#ifndef __ADC_H
#define __ADC_H

#include "esp_adc/adc_oneshot.h"

// 初始化
void adc_init(adc_channel_t adc_channel);

// 读取raw值
void  ADC_Read_Raw(int *ADC_Data , adc_channel_t adc_Channel);

#endif
