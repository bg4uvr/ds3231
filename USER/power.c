#include "power.h"
//#include "led.h"

//1Hz信号端口及引脚号
#define SIG_1Hz_PORT    GPIOB
#define SIG_1Hz_PIN     GPIO_PIN_2
#define SIG_1Hz_EXTI_PORT   EXTI_PORT_GPIOB

//KEY1端口及引脚号
#define KEY1_PORT       GPIOB
#define KEY1_PIN        GPIO_PIN_3
#define KEY1_EXTI_PORT  EXTI_PORT_GPIOB

//KEY2端口及引脚号
#define KEY2_PORT       GPIOD
#define KEY2_PIN        GPIO_PIN_3
#define KEY2_EXTI_PORT  EXTI_PORT_GPIOD

bool halfsec;               //半秒标志
bool key_flag;              //按键标志

//初始化电源管理
void power_init(void)
{
	//LED_Init(LED3);			//初始化LED

	//设置1Hz信号脚为输入，开上拉，并开启中断
	GPIO_Init(SIG_1Hz_PORT, SIG_1Hz_PIN, GPIO_MODE_IN_PU_IT);

	//设置1Hz中断为上升下降沿触发
	EXTI_SetExtIntSensitivity(SIG_1Hz_EXTI_PORT, EXTI_SENSITIVITY_RISE_FALL);

	//使能所有中断
	enableInterrupts();
}

//进入停机模式
void power_halt(void)
{
	//保存端口方向状态
	uint8_t
	DDRa=GPIOA->DDR,
	DDRb=GPIOB->DDR,
	DDRc=GPIOC->DDR,
	DDRd=GPIOD->DDR,
	DDRe=GPIOE->DDR;

	//除无关端口外，全部设置为输入
	GPIOA->DDR=GPIO_PIN_1|GPIO_PIN_2;       //晶振脚设置为输出并拉低，以降低功耗
	GPIOB->DDR=0;
	GPIOC->DDR=GPIO_PIN_5|GPIO_PIN_6;       //LED的VCC和GND
	GPIOD->DDR=00;//GPIO_PIN_2;                  //LED3
	GPIOE->DDR=0;

	//更改两个按键的端口为下沿中断模式，开启上拉
	GPIO_Init(KEY1_PORT, KEY1_PIN, GPIO_MODE_IN_PU_IT);
	GPIO_Init(KEY2_PORT, KEY2_PIN, GPIO_MODE_IN_PU_IT);
	EXTI_SetExtIntSensitivity(KEY1_EXTI_PORT, EXTI_SENSITIVITY_FALL_ONLY);
	EXTI_SetExtIntSensitivity(KEY2_EXTI_PORT, EXTI_SENSITIVITY_FALL_ONLY);

	//关灯待机
	//LED_Off(LED3);
	halt();
	//LED_On(LED3);

	//唤醒恢复端口方向（此时自动关闭了后设置的按键中断）
	GPIOA->DDR=DDRa;
	GPIOB->DDR=DDRb;
	GPIOC->DDR=DDRc;
	GPIOD->DDR=DDRd;
	GPIOE->DDR=DDRe;
}

//B端口外部中断
INTERRUPT_HANDLER(EXTI_PORTB_IRQHandler, 4)
{
	//读取半秒状态
	halfsec = (bool)GPIO_ReadInputPin(SIG_1Hz_PORT,SIG_1Hz_PIN);

	//如果按键1为低电位，设置按键标志
	if(!(bool)GPIO_ReadInputPin(KEY1_PORT,KEY1_PIN))
	{
		//关闭按键中断，恢复为普遍输入模式
		GPIO_Init(KEY1_PORT,KEY1_PIN, GPIO_MODE_IN_FL_NO_IT);
		//设立按键标志
		key_flag=TRUE;
	}
}

//D端口外部中断
INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
	//如果按键2为低电位，设置按键标志
	if(!(bool)GPIO_ReadInputPin(KEY2_PORT,KEY2_PIN))
	{
		//关闭按键中断，恢复为普遍输入模式
		GPIO_Init(KEY2_PORT,KEY2_PIN, GPIO_MODE_IN_FL_NO_IT);
		//设立按键标志
		key_flag=TRUE;
	}
}