#ifndef __MYTASK_H__
#define __MYTASK_H__   

#include "Initial.h"

// ================= 全局声明 =================
// 初始化外设
void main_Initial(void) ;
// 打印FreeRtos参数
void FreeRtos_Task_Print(void) ;

// ================= 任务调度控制器 =================
void Task_Create(void);

#endif
