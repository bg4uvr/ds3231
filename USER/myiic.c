#include "myiic.h"
#include "delay.h"

//初始化IIC
void IIC_Init(void)
{
	GPIO_Init(IIC_PORT, IIC_SCL, GPIO_MODE_OUT_OD_HIZ_FAST);
	GPIO_Init(IIC_PORT, IIC_SDA, GPIO_MODE_OUT_OD_HIZ_FAST);
}

//产生IIC起始信号
void IIC_Start(void)
{
	IIC_SDA_1;
	IIC_SCL_1;
	delay(1);
	IIC_SDA_0;		//START:when CLK is high,DATA change form high to low
	delay(1);
	IIC_SCL_0;		//钳住I2C总线，准备发送或接收数据
}

//产生IIC停止信号
void IIC_Stop(void)
{
	IIC_SCL_0;
	IIC_SDA_0;		//STOP:when CLK is high DATA change form low to high
	delay(1);
	IIC_SCL_1;
	delay(1);
	IIC_SDA_1;		//发送I2C总线结束信号
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	IIC_SDA_1;
	IIC_SCL_1;
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_0;		//时钟输出0
	return 0;
}

//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL_0;
	IIC_SDA_0;
	delay(1);
	IIC_SCL_1;
	delay(1);
	IIC_SCL_0;
}

//不产生ACK应答
void IIC_NAck(void)
{
	IIC_SCL_0;
	IIC_SDA_1;
	delay(1);
	IIC_SCL_1;
	delay(1);
	IIC_SCL_0;
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC_Send_Byte(uint8_t txd)
{
	uint8_t t;
	IIC_SCL_0;		//拉低时钟开始数据传输
	for(t=0; t<8; t++)
	{
		if(txd&0x80)
		{
			IIC_SDA_1;
		}
		else
		{
			IIC_SDA_0;
		}
		txd<<=1;
		delay(1);	//对TEA5767这三个延时都是必须的
		IIC_SCL_1;
		delay(1);
		IIC_SCL_0;
		delay(1);
	}
}

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
uint8_t IIC_Read_Byte(uint8_t ack)
{
	uint8_t i,receive=0;
	IIC_SDA_1;		//释放SDA总线
	for(i=0; i<8; i++ )
	{
		IIC_SCL_0;
		delay(1);
		IIC_SCL_1;
		receive<<=1;
		delay(1);
		if(READ_SDA)
			receive++;
		delay(1);
	}
	if (!ack)
		IIC_NAck();	//发送nACK
	else
		IIC_Ack();	//发送ACK
	return receive;
}



























