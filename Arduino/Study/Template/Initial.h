#ifndef TEMPLATE_INITIAL_H
#define TEMPLATE_INITIAL_H

// 包含所有头文件,方便后续使用
#include <Arduino.h>
#include "src/Hardware/LED.h"

#ifdef __cplusplus
extern "C" {
#endif

void Initial_All(void);

// 打印全部任务的状态、核亲和性、历史最小剩余栈(字节)和约1秒采样的CPU占用。
// 仅在一个普通任务中低频调用；函数会阻塞调用者约1秒，其他任务继续运行。
// 调试时避免与任务删除并发：快照中的任务名称指针由FreeRTOS任务持有。
void print_FreeRtos_Task(void);

#ifdef __cplusplus
}
#endif

#endif
