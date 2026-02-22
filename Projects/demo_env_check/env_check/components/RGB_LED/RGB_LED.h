#ifndef _RGB_LED_H_
#define _RGB_LED_H_

#include "stdbool.h"

// 有两个是与USART0连接,暂时不用
// #define RGB_LED_R_GPIO_NUM GPIO_NUM_43  // D6
// #define RGB_LED_G_GPIO_NUM GPIO_NUM_44  // D7
#define RGB_LED_B_GPIO_NUM GPIO_NUM_7   // D8

void RGB_LED_Init(void);
void RGB_LED_Set(bool is_R_ON , bool is_G_ON ,bool is_B_ON) ;

#endif // RGB_LED
