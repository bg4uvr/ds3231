#ifndef __LED_H
#define __LED_H

#include "stm8s.h"

//定义LED名称
#define  LED1       0x01
#define  LED2       0x02
#define  LED3       0x04

//定义LED接口
#define LED1_GPIO_PORT  (GPIOC)
#define LED1_GPIO_PINS  (GPIO_PIN_3)

#define LED2_GPIO_PORT  (GPIOC)
#define LED2_GPIO_PINS  (GPIO_PIN_4)

#define LED3_GPIO_PORT  (GPIOD)
#define LED3_GPIO_PINS  (GPIO_PIN_2)

//声明功能函数
extern void LED_Init(uint8_t LedNum);
extern void LED_On(uint8_t LedNum);
extern void LED_Off(uint8_t LedNum);
extern void LED_Toggle(uint8_t LedNum);

#endif
