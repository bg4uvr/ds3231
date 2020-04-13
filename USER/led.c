
#include "led.h"



/*******************************************************************************
****函数名称:
****函数功能:初始化LED接口
****版本:V1.0
****日期:14-2-2014
****入口参数:需要初始化的LED
取值下列值之一或者它们之间的“或运算”组合形式
            LED1
            LED2
            LED3
****出口参数:无
****说明:
********************************************************************************/
void LED_Init(uint8_t LedNum)
{
	if(LedNum&LED1)
	{
		GPIO_Init(LED1_GPIO_PORT, (GPIO_Pin_TypeDef)LED1_GPIO_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
	}
	if(LedNum&LED2)
	{
		GPIO_Init(LED2_GPIO_PORT, (GPIO_Pin_TypeDef)LED2_GPIO_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
	}
	if(LedNum&LED3)
	{
		GPIO_Init(LED3_GPIO_PORT, (GPIO_Pin_TypeDef)LED3_GPIO_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
	}
}
/*******************************************************************************
****函数名称:
****函数功能:点亮LED
****版本:V1.0
****日期:14-2-2014
****入口参数:需要点亮的LED
取值下列值之一或者它们之间的“或运算”组合形式
            LED1
            LED2
            LED3
****出口参数:无
****说明:
********************************************************************************/
void LED_On(uint8_t LedNum)
{
	if(LedNum&LED1)
	{
		GPIO_WriteLow(LED1_GPIO_PORT, LED1_GPIO_PINS);
	}
	if(LedNum&LED2)
	{
		GPIO_WriteLow(LED2_GPIO_PORT, LED2_GPIO_PINS);
	}
	if(LedNum&LED3)
	{
		GPIO_WriteLow(LED3_GPIO_PORT, LED3_GPIO_PINS);
	}
}
/*******************************************************************************
****函数名称:
****函数功能:熄灭LED
****版本:V1.0
****日期:14-2-2014
****入口参数:需要熄灭的LED
取值下列值之一或者它们之间的“或运算”组合形式
            LED1
            LED2
            LED3
****出口参数:无
****说明:
********************************************************************************/
void LED_Off(uint8_t LedNum)
{
	if(LedNum&LED1)
	{
		GPIO_WriteHigh(LED1_GPIO_PORT, LED1_GPIO_PINS);
	}
	if(LedNum&LED2)
	{
		GPIO_WriteHigh(LED2_GPIO_PORT, LED2_GPIO_PINS);
	}
	if(LedNum&LED3)
	{
		GPIO_WriteHigh(LED3_GPIO_PORT, LED3_GPIO_PINS);
	}
}
/*******************************************************************************
****函数名称:
****函数功能:翻转LED
****版本:V1.0
****日期:14-2-2014
****入口参数:需要翻转的LED
取值下列值之一或者它们之间的“或运算”组合形式
            LED1
            LED2
            LED3
****出口参数:无
****说明:
********************************************************************************/
void LED_Toggle(uint8_t LedNum)
{
	if(LedNum&LED1)
	{
		GPIO_WriteReverse(LED1_GPIO_PORT, LED1_GPIO_PINS);
	}
	if(LedNum&LED2)
	{
		GPIO_WriteReverse(LED2_GPIO_PORT, LED2_GPIO_PINS);
	}
	if(LedNum&LED3)
	{
		GPIO_WriteReverse(LED3_GPIO_PORT, LED3_GPIO_PINS);
	}
}
