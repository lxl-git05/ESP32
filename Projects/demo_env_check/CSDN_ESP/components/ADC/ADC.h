#ifndef __ADC_H
#define __ADC_H

#include "esp_adc/adc_oneshot.h"

#define ADC_GPIO_NUM 9  
#define ADC_Channel ADC_CHANNEL_8   // ADC_CHANNEL_8∂‘”¶GPIO9 - D10

// ADC???
void adc_init(void) ;

// raw??
void  ADC_Read_Raw(int *ADC_Data) ;

#endif
