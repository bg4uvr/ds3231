#ifndef __MYIIC_H
#define __MYIIC_H
#include "stm8s.h"

//软件IIC端口定义
#define IIC_PORT	GPIOB
#define IIC_SCL		GPIO_PIN_4		//SCL
#define IIC_SDA    	GPIO_PIN_5		//SDA

#define READ_SDA   	GPIO_ReadInputPin(IIC_PORT,IIC_SDA)

#define	IIC_SCL_1	GPIO_WriteHigh(IIC_PORT, IIC_SCL);
#define	IIC_SCL_0	GPIO_WriteLow(IIC_PORT, IIC_SCL);
#define	IIC_SDA_1	GPIO_WriteHigh(IIC_PORT, IIC_SDA);
#define	IIC_SDA_0	GPIO_WriteLow(IIC_PORT, IIC_SDA);


//IIC所有操作函数
void IIC_Init(void);				//初始化IIC的IO口
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);	//IIC发送一个字节
uint8_t IIC_Read_Byte(uint8_t ack);	//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 		//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);

#endif



