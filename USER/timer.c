#include "timer.h"
#include "led.h"
#include "button.h"

uint16_t ModeTimeOutCnt;    //模式超时计数器

//定时器4初始化
void Timer4_Init(void)
{
	TIM4_DeInit();
	TIM4_TimeBaseInit(TIM4_PRESCALER_128,155);	//2MHz 128分频= 15625Hz，重装值155，溢出周期约为10mS
	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
	TIM4_Cmd(ENABLE);
	enableInterrupts();
}

//定时器4中断，溢出时间10毫秒
INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
	TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
	Button_Process();		    //需要周期调用按键处理函数
	if(ModeTimeOutCnt<510)  ModeTimeOutCnt++;		//模式超时计数器
}