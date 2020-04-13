/********************************************************************************

	STM32F103C8T6模块 + DS3231 + 24C32）模块 + TM1637数码管模块 + OLED显示模块

	《实时时钟 + 温度测量并保存，实时OLED显示温度曲线，数码管实际显示时钟》

															的，实验程序……

														2019.2.3完成 by bg4uvr~
2020.2.8:
1、加入电源管理，空闲时进入掉电模式，调试完成。

********************************************************************************/


#include "ds3231.h"
#include "myiic.h"
#include "stdlib.h"                     //用于malloc
#include "string.h"                     //用于memset

//BCD转HEX
inline uint8_t bcd2hex(uint8_t bcd_data)
{
    return ((bcd_data/16)*10+bcd_data%16);
}

//HEX转BCD
inline uint8_t hex2bcd(uint8_t hex_data)
{
    return ((hex_data/10)*0x10+hex_data%10);
}

//计算是否是闰年(输入数值0-199，代表2000-2199年)
bool isLeap(uint8_t year)
{
    unsigned int y=year+2000;
    if((y%4==0 && y%100!=0) || y%400==0)
        return TRUE;
    else
        return FALSE;
}

//计算某年的某月有多少天(年份输入数值0-199，代表2000-2199年)
uint8_t max_date(uint8_t year,uint8_t month)
{
    const uint8_t month_tab[]={31,28,31,30,31,30,31,31,30,31,30,31};
    return isLeap(year)&&month==2?month_tab[month-1]+1:month_tab[month-1];
}

//DS3231读取函数
void DS3231_Read(uint8_t Reg_add, uint8_t byte_cnt, uint8_t *buf)
{
	IIC_Start();						//发送I2C START信号
	IIC_Send_Byte(DS3231_WRITE_ADD);	//发送DS3231硬件写地址
	IIC_Wait_Ack();						//等待ACK回应
	IIC_Send_Byte(Reg_add);				//发送寄存器地址
	IIC_Wait_Ack();						//等待ACK回应
	IIC_Start();						//发送I2C START信号
	IIC_Send_Byte(DS3231_READ_ADD);		//发送DS3231硬件读地址
	IIC_Wait_Ack();						//等待ACK回应
	while(--byte_cnt)					//如果不是最后1字节数据
	{
		*(buf++)=IIC_Read_Byte(1);		//读取并发送ACK
	}
	*buf=IIC_Read_Byte(0);				//读取并发送NACK
	IIC_Stop();							//发送I2C STOP信号
}

//DS3231写入函数
void DS3231_Write(uint8_t Reg_add, uint8_t byte_cnt, uint8_t *buf)
{
	IIC_Start();						//发送I2C START信号
	IIC_Send_Byte(DS3231_WRITE_ADD);	//发送DS3231硬件写地址
	IIC_Wait_Ack();						//等待ACK回应
	IIC_Send_Byte(Reg_add);				//发送寄存器地址
	IIC_Wait_Ack();						//等待ACK回应
	while(byte_cnt--)					//如果仍有未发送数据
	{
		IIC_Send_Byte(*(buf++));		//发送之
		IIC_Wait_Ack();					//等待ACK信号
	}
	IIC_Stop();							//发送I2C STOP信号
}

//立即测量温度
int8_t DS3231_Read_temp(void)
{
	uint8_t buf[2];

	//等待空闲
	do
	{
		DS3231_Read(CONTROL,2,buf);
	}
	while(buf[1]&BSY);

	//开始测量温度
	buf[0]|=CONV;
	DS3231_Write(CONTROL,1,buf);

	//等待测量完成
	do
	{
		DS3231_Read(CONTROL,1,buf);
	}
	while(buf[0]&CONV);

	//读取温度
	DS3231_Read(TEMP_H,2,buf);

	//转换温度格式，2字节压缩为1字节
	int8_t result = (buf[0]<<2)|(buf[1]>>6);

	//如果是负温度，恢复负号
	if(buf[0]&0x80)	result |= 0x80;

	//如果压缩过程存在溢出，限制成为最大或最小值
	if(buf[0]&0x40)
	{
		if(result < 0)	result = -128;
		else			result = 127;
	}

	return(result);
}

//计算星期(输入为BCD数，适用于2000-2099年)
uint8_t week(uint8_t y, uint8_t m, uint8_t d)
{
	uint16_t year=(y>>4)*10+(y&0x0f)+2000;
	m=(m>>4)*10+m&0x0f;
	d=(d>>4)*10+d&0x0f;
	if(m<3)
	{
		m+=12;
		year--;
	}
	return((d+2*m+3*(m+1)/5+year+year/4+year/400-year/100)%7+1);
}

//DS3231初始化程序
void DS3231_init(void)
{
	uint8_t flag;

	//读取状态字节
	DS3231_Read(CTL_STA,1,&flag);

	//如果停机标志置位，说明初次上电或者其他原因曾经停机，重新设置寄存器
	if(flag&OSF)
	{
		uint8_t* ds3231 = malloc(sizeof(uint8_t)*0x10);	//申请17字节RAM
		if(ds3231!=NULL)
		{
			memset(ds3231,0,0x10);		//清零RAM

			//设置默认日期和时间
			ds3231[YEAR]=0x20;
			ds3231[MONTH]=0x01;
			ds3231[DATE]=0x01;
			ds3231[WEEK]=week(ds3231[YEAR],ds3231[MONTH],ds3231[DATE]);
			//ds3231[HOUR]=0x14;

			//设置SQW脚输出1Hz方波输出，停止32KHz输出。电池供电时时钟运行，方波停止，闹钟关闭
			//ds3231[CTRL]=0;
			//ds3231[CTRL_STA]=0;

			//SQW脚设置为闹钟输出，开启32KHz时钟输出（厂家默认上电设置）
			//ds3231[CTRL]=INTCN;
			//ds3231[CTRL_STA]=EN32kHz;

			DS3231_Write(0,0x10,ds3231);//写入DS3231
			free(ds3231);
		}
	}
	//非掉电情况，只重新设置1Hz方波输出
	else
	{
		u8 temp;
		DS3231_Read(CONTROL,1,&temp);
		temp &= ~RS2 & ~RS1 & ~INTCN;
		DS3231_Write(CONTROL,1,&temp);
	}
}

