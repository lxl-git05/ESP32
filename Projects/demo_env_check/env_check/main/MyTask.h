#ifndef __MYTASK_H__
#define __MYTASK_H__   

#include "Initial.h"

// ================= 全局声明 =================
// 初始化外设
void main_Initial(void) ;
// 打印FreeRtos参数
void print_FreeRtos_Task(void) ;

// ================= 外部任务调度 =================
void task1(void *param);
void Task_DHT11(void *param);
void Task_ADC(void *param);
void Task_OLED(void *param);

// ================= 任务调度控制器 =================
void Msg_Create(void);
void Task_Create(void);

#endif
