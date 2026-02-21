#ifndef __ADC_APP_H
#define __ADC_APP_H

#include "esp_err.h"

// =============== 数据层 ===============
#define ADC_Debug
#define Key_Channel  ADC_CHANNEL_0   // ADC_CHANNEL_0 -> GPIO1 - D0
#define Gray_Channel ADC_CHANNEL_1   // ADC_CHANNEL_1 -> GPIO2 - D1
// =============== 数据层 ===============

// 光强传感器
void ADC_Gray_Init(void);
void ADC_Get_Gray(int *Gray_Data);
// 4按键传感器
void ADC_Key_Init(void);
void ADC_Get_Key(int *Gray_Data);

#endif
