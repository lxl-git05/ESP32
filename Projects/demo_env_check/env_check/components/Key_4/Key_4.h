#ifndef __ADC_APP_H
#define __ADC_APP_H

#include "ADC.h"

typedef enum
{
    Key_NULL     ,
    KEY_1_Single ,
    KEY_2_Single ,
    KEY_3_Single ,
    KEY_4_Single ,
}Key_Status_Typedef ;

// =============== 数据层 ===============
#define Key_Channel  ADC_CHANNEL_0   // ADC_CHANNEL_0 -> GPIO1 - D0
extern Key_Status_Typedef Key_Status ;
// =============== 数据层 ===============

// 4按键传感器
void Key_4_Init(void);
void Key_4_Get(int *Gray_Data);
Key_Status_Typedef Key_4_Get_Status(void) ;
// 任务
void Task_Key4(void *param) ;
#endif
