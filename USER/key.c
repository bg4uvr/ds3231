
#include "key.h"
#include "delay.h"

void KEY_Init(void)
{
	GPIO_Init(KEY1_GPIO_PORT, KEY1_GPIO_PINS, GPIO_MODE_IN_FL_NO_IT);
	GPIO_Init(KEY2_GPIO_PORT, KEY2_GPIO_PINS, GPIO_MODE_IN_FL_NO_IT);
	GPIO_Init(KEY3_GPIO_PORT, KEY3_GPIO_PINS, GPIO_MODE_IN_FL_NO_IT);
}

uint8_t Read_KEY1_Level(void)
{
	return (GPIO_ReadInputPin(KEY1_GPIO_PORT, KEY1_GPIO_PINS))?1:0;
}

uint8_t Read_KEY2_Level(void)
{
	return (GPIO_ReadInputPin(KEY2_GPIO_PORT, KEY2_GPIO_PINS))?1:0;
}

uint8_t Read_KEY3_Level(void)
{
	return (GPIO_ReadInputPin(KEY3_GPIO_PORT, KEY3_GPIO_PINS))?1:0;	
}
