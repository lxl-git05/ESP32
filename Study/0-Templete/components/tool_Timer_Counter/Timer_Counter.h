#ifndef __TIMER_COUNTER_H
#define __TIMER_COUNTER_H

#include <stdio.h>
#include "stdint.h"     // uint需要
#include "esp_timer.h"  // 系统自增定时器

// 示范:

//Timer_Counter_Begin() ;
// func()	
//Timer_Counter_End() ;

extern uint64_t time_us ;				// 代码之间的时间间隔
extern uint64_t time_Func_us ;		    // 函数两次执行的时间间隔

// 计时器开始计时
void Timer_Counter_Begin(void);
// 计时器结束计时
void Timer_Counter_End(void);
// 计算一个多次执行的函数的每次执行间隔时间
void Timer_Counter_Func(void) ;
// 打印展示
void Timer_Counter_Print(void) ;

#endif
