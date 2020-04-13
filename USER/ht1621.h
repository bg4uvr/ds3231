#ifndef __HT1621_H
#define __HT1621_H

#include "stm8s.h"

//模块接口定义
#define ht1621_CS_PORT	(GPIOE)
#define ht1621_CS_PIN	(GPIO_PIN_5)
#define ht1621_WR_PORT	(GPIOC)
#define ht1621_WR_PIN 	(GPIO_PIN_1)
#define ht1621_DAT_PORT	(GPIOC)
#define ht1621_DAT_PIN 	(GPIO_PIN_3)
#define ht1621_GND_PORT	(GPIOC)
#define ht1621_GND_PIN	(GPIO_PIN_6)
#define ht1621_VCC_PORT	(GPIOC)
#define ht1621_VCC_PIN 	(GPIO_PIN_5)

void ht1621Init(void);

void dispByte(uint8_t addr,uint8_t data);
void dispAll(uint8_t* dspbuf);

#endif