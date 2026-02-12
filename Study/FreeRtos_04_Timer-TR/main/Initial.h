#ifndef __INITIAL_H__
#define __INITIAL_H__   

// 系统层
#include <stdio.h>
#include "stdbool.h"
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 工具层
#include "Timer_Counter.h"

// 外设API
#include "driver/gpio.h"

// 外设模块
#include "MyLED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
#include "PWM.h"
#include "MPU6050.h"
#include "MPU6050_Angle.h"

// WIFI模块


// ================= 全局声明 =================
void Initial(void) ;

void print_FreeRtos_Task(void) ;

#endif
