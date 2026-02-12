#ifndef __BUTTON_H
#define __BUTTON_H

#include "driver/gpio.h"
#include "stdint.h"         // uint8_t需要

// 按键GPIO(GPIOx和Dx不同,一定要记得!!!)
#define Key_0_GPIO_PIN GPIO_NUM_4

// ================ 按键状态检测相关宏定义 ================
#define KEY_COUNT				4

#define KEY_0					0
// #define KEY_1					1
// #define KEY_2  				    2
// #define KEY_3					3

#define KEY_HOLD				0x01
#define KEY_DOWN				0x02
#define KEY_UP					0x04
#define KEY_SINGLE			    0x08
#define KEY_DOUBLE			    0x10
#define KEY_LONG				0x20

#define KEY_REPEAT				0x40

// ================ 按键相关函数 ================
void Key_Init(void);
uint8_t Key_Check(uint8_t n, uint8_t Flag);
void Key_Tick(void);

#endif