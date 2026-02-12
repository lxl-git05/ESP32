#include "Timer_Counter.h"

uint64_t Timer_Counter_Start_count = 0 ;
uint64_t Timer_Counter_End_count = 0 ;
uint64_t time_us = 0 ;
uint64_t time_Func_us = 0 ;

void Timer_Counter_Begin(void)
{
	Timer_Counter_Start_count = esp_timer_get_time();	// 起始时间
}

void Timer_Counter_End()
{
	Timer_Counter_End_count = esp_timer_get_time() ;	            // 终止时间
	time_us = Timer_Counter_End_count - Timer_Counter_Start_count;	// 间隔时间	
}

void Timer_Counter_Func(void)
{
	static uint64_t Timer_Counter_Before = 0;	        // 上次时间
	
	uint64_t Timer_Counter_Now = esp_timer_get_time();	// 当前时间
	
	time_Func_us = Timer_Counter_Now - Timer_Counter_Before;					// 计算间隔时间	

	Timer_Counter_Before = Timer_Counter_Now ;	// 更新上次时间
}

void Timer_Counter_Print(void)
{
	printf("time_us:%lld\n" , time_us);
	printf("time_Func_us:%lld\n" , time_Func_us);
}
