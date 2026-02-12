#ifndef _MYLED_H_
#define _MYLED_H_

#define LED_GPIO_NUM GPIO_NUM_21 // ∞Â‘ÿLED

void LED_Init(void);

void LED_Write(int level);

void LED_On(void);

void LED_OFF(void);

#endif // MYLED_H