#ifndef __USART_H
#define __USART_H

#include "driver/uart.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stdbool.h"
#include <esp_log.h>
#include <driver/gpio.h>    

#include "Timer_Counter.h"

// 参数
#define USART_PORT_0 UART_NUM_1  // 0号不能使用,GPIO43和GPIO44水平不够不要复用为别的模式
#define USART_TX_PIN GPIO_NUM_7  //D8
#define USART_RX_PIN GPIO_NUM_8  //D9
#define USART_BUFFER_SIZE 2048

// 函数
// 初始化
void USART_Init(void);
// 发送消息
int sendData(const char* data);
// 接收数据:阻塞式
void ReceiveData_Zuse(char recData[]) ;
#endif
