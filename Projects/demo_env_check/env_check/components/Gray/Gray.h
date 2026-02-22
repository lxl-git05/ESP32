#ifndef __Gray_H
#define __GRAY_H

#include "ADC.h"

// =============== 数据层 ===============
#define Gray_Channel ADC_CHANNEL_1   // ADC_CHANNEL_1 -> GPIO2 - D1
// =============== 数据层 ===============

// 光强传感器
void Gray_Init(void);
void Gray_Get(int *Gray_Data);
// 任务声明
void Task_Gray(void *param) ;
#endif
