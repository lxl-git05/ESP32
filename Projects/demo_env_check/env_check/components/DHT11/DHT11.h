#ifndef __DHT11_H
#define __DHT11_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h" 
 
#define DHT_Debug

/* 引脚定义 */
#define DHT11_DQ_GPIO_PIN       GPIO_NUM_3      // D2引脚
 
extern uint8_t temp ;  // 温度
extern uint8_t humi ;  // 湿度

/* DHT11引脚高低电平枚举 */
typedef enum 
{
    DHT11_PIN_RESET = 0u,
    DHT11_PIN_SET
}DHT11_GPIO_PinState;
 
/* IO操作 */
#define DHT11_DQ_IN     gpio_get_level(DHT11_DQ_GPIO_PIN)   /* 数据端口输入 */
 
/* DHT11端口定义 */
#define DHT11_DQ_OUT(x) do{ x ?                                                 \
                            gpio_set_level(DHT11_DQ_GPIO_PIN, DHT11_PIN_SET) :  \
                            gpio_set_level(DHT11_DQ_GPIO_PIN, DHT11_PIN_RESET); \
                        }while(0)
 
/* 函数声明 */
void dht11_reset(void);                                 /* 复位DHT11 */
uint8_t dht11_init(void);                               /* 初始化DHT11 */
uint8_t dht11_check(void);                              /* 等待DHT11的回应 */
uint8_t dht11_read_data(uint8_t *temp,uint8_t *humi);   /* 读取温湿度 */
void dht11_task_tick(uint16_t vtaskdelay_ms) ;          /* 任务执行函数 */
#endif
