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

// WIFI模块
#include "Wifi_Manager.h"   
#include "Wifi_MQTT.h"

// ================= 全局声明 =================
void Initial(void) ;

#endif
