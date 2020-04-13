#ifndef _key_h_
#define _key_h_
#include "stm8s.h"

//定义按键接口
#define KEY1_GPIO_PORT  (GPIOB)
#define KEY1_GPIO_PINS  (GPIO_PIN_3)

#define KEY2_GPIO_PORT  (GPIOD)
#define KEY2_GPIO_PINS  (GPIO_PIN_3)

#define KEY3_GPIO_PORT  (GPIOD)
#define KEY3_GPIO_PINS  (GPIO_PIN_7)

//定义按键有效电平
#define KEY_ON	0

void KEY_Init(void);

uint8_t Read_KEY1_Level(void);
uint8_t Read_KEY2_Level(void);
uint8_t Read_KEY3_Level(void);

#endif
