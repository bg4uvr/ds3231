/********************************************************************************

	STM32F103C8T6模块 + DS3231 + 24C32）模块 + TM1637数码管模块 + OLED显示模块

	《实时时钟 + 温度测量并保存，实时OLED显示温度曲线，数码管实际显示时钟》

															的，实验程序……

														2019.2.3完成 by bg4uvr~

********************************************************************************/



#include "24cxx.h"
#include "delay.h"

//Mini STM32开发板
//24CXX驱动函数(适合24C01~24C16,24C32~256未经过测试!有待验证!)
//正点原子@ALIENTEK
//2010/6/10
//V1.2

//初始化IIC接口
void AT24CXX_Init(void)
{
	IIC_Init();
}

//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址
//返回值  :读到的数据
uint8_t AT24CXX_ByteRead(uint16_t ReadAddr)
{
	uint8_t temp=0;
	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(EE_ADD);		//发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);	//发送高地址
	}
	else
		IIC_Send_Byte(0xA0 + ((ReadAddr/256)<<1));   //发送器件地址0xA0,写数据

	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr%256);	//发送低地址
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(EE_ADD|0x01);		//进入接收模式
	IIC_Wait_Ack();
	temp=IIC_Read_Byte(0);
	IIC_Stop();						//产生一个停止条件
	return temp;
}

//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址
//DataToWrite:要写入的数据
void AT24CXX_ByteWrite(uint16_t WriteAddr, uint8_t DataToWrite)
{
	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(EE_ADD);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址
	}
	else
		IIC_Send_Byte(EE_ADD + ((WriteAddr/256)<<1));   //发送器件地址0xA0,写数据
	IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack();
	IIC_Send_Byte(DataToWrite);     //发送字节
	IIC_Wait_Ack();
	IIC_Stop();						//产生一个停止条件
	delay_ms(2);
}

//页内写入
void AT24CXX_PageWrite(uint16_t WriteAddr,  uint8_t Len, uint8_t *buf)
{
	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(EE_ADD);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址
	}
	else
		IIC_Send_Byte(EE_ADD + ((WriteAddr/256)<<1));   //发送器件地址0xA0,写数据
	IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack();
	while(Len--)
	{
		IIC_Send_Byte(*(buf++));	//发送字节
		IIC_Wait_Ack();
	}
	IIC_Stop();						//产生一个停止条件
	delay_ms(10);
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//ReadAddr   :开始读出的地址
//Len        :要读出数据的长度
void AT24CXX_Read(uint16_t ReadAddr, uint16_t Len, uint8_t *buf)
{
	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(EE_ADD);		//发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);	//发送高地址
	}
	else
		IIC_Send_Byte(EE_ADD + ((ReadAddr/256)<<1));   //发送器件地址0xA0,写数据

	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr%256);	//发送低地址
	IIC_Wait_Ack();

	IIC_Start();
	IIC_Send_Byte(EE_ADD|0x01);		//进入接收模式
	IIC_Wait_Ack();
	while(--Len)
	{
		*(buf++)=IIC_Read_Byte(1);
	}
	*buf=IIC_Read_Byte(0);
	IIC_Stop();						//产生一个停止条件
}

//连续写入
void AT24CXX_Write(uint16_t ReadAddr, uint16_t Len, uint8_t *buf)
{
	uint8_t i;		//第1个目标页内剩余的空间字节数
	uint8_t j;		//需要整页写入的页数
	uint8_t k;		//最后一页需要写入的字节数
	uint8_t l;

	i = PAGE_SIZE - ReadAddr%PAGE_SIZE;			//第1个目标页内剩余的空间字节数

	if(Len <= i)								//如果需要写入的总字节数小于当前PAGE剩余字节数
		AT24CXX_PageWrite(ReadAddr,Len,buf);	//直接页内写入即可
	else										//数据需要跨页
	{
		AT24CXX_PageWrite(ReadAddr,i,buf);		//写入第1页内数据

		j = (Len-i)/PAGE_SIZE;					//计算需要整页写入的页数
		k = (Len-i)%PAGE_SIZE;					//计算最后一页需要写入的字节数

		ReadAddr += i;							//计算新的地址偏移
		buf += i;								//数据指针指向新的偏移

		if(j>0)									//如果仍需整页写入的页数大于0
		{
			for(l=0; l<j; l++)					//写入相应的页数
			{
				AT24CXX_PageWrite(ReadAddr,PAGE_SIZE,buf);
				ReadAddr += PAGE_SIZE;
				buf += PAGE_SIZE;
			}
		}
		if(k>0)
			AT24CXX_PageWrite(ReadAddr,k,buf);	//写入最后一页的数据
	}
}










