/********************************************************************************

	STM32F103C8T6模块 + DS3231 + 24C32）模块 + TM1637数码管模块 + OLED显示模块

	《实时时钟 + 温度测量并保存，实时OLED显示温度曲线，数码管实际显示时钟》

															的，实验程序……

														2019.2.3完成 by bg4uvr~

********************************************************************************/



#ifndef __DS3231_H
#define __DS3231_H
#include "stm8s.h"

//DS3231硬件地址
#define DS3231_WRITE_ADD	0xD0		//
#define DS3231_READ_ADD		0xD1

//DS3231寄存器定义
#define	SEC				0x00
#define	MIN				0x01
#define HOUR			0x02
#define WEEK			0x03
#define DATE			0x04
#define MONTH			0x05
#define YEAR			0x06
#define AL1_SEC			0x07
#define AL1_MIN			0x08
#define AL1_HOUR		0x09
#define AL1_DAY_DATE	0x0A
#define AL2_MIN			0x0B
#define AL2_HOUR		0x0C
#define AL2_DAY_DATE	0x0D
#define CONTROL			0x0E
	//0x0E控制位
	#define	EOSC		(1<<7)		//电池供电时，时钟是否运行，0:运行，1:停止，VCC时此位无效一直运行，初次上电时默认为0。
	#define BBSQW		(1<<6)      //电池供电时，INT/SQW脚是否输出方波，0:高阻态，1:输出方波，初次上电为0。
	#define	CONV		(1<<5)      //置1强制测量温度（平时64秒自动测量一次）
	#define RS2			(1<<4)		//方波频率选择，RS2:RS1	00=1Hz, 01=1024Hz, 10=4096Hz, 11=8192Hz
	#define RS1			(1<<3)
	#define INTCN		(1<<2)      //“INT/SQW”脚功能设置，0输出方波，1为闹钟中断
	#define A2IE		(1<<1)      //闹钟2使能
	#define A1IE		(1<<0)      //闹钟1使能
#define CTL_STA			0x0F
	//0x0F控制&状态位
	#define	OSF			(1<<7)      //时钟停止标志位。首次上电、VCC和BAT同时掉电、电池供电时EOSC置1、外部干扰等，可造成置位。置位状态需要人工清零。
	#define EN32kHz		(1<<3)      //32K脚输出设置位，电源供电时，1输出32K，0高阻态。上电时此位为1，32K脚输出方波。
	#define BSY			(1<<2)      //温度测量忙标志
	#define A2F			(1<<1)      //闹钟2标志
	#define A1F			(1<<0)      //闹钟1标志
#define AGING_OFFSET	0x10        //校准值，正值增加电容，负值减小电容，25度时每1LSB对频率大约影响0.1ppm。
#define TEMP_H			0x11        //温度整数位（有符号）
#define TEMP_L			0x12        //温度小数位（最左侧2位，所以分辨率为0.25度）

//DS3231寄存器结构体	( 19Bytes )
typedef struct
{
	uint8_t	sec;
	uint8_t min;
	uint8_t hour;
	uint8_t week;
	uint8_t date;
	uint8_t month;
	uint8_t year;
//	uint8_t al1_sec;
//	uint8_t al1_min;
//	uint8_t al1_hour;
//	uint8_t al1_week;
//	uint8_t al2_min;
//	uint8_t al2_hour;
//	uint8_t al2_week;
//	uint8_t ctrl;
//	uint8_t ctrl_sta;
//	uint8_t offset;
	uint8_t temp_h;
	uint8_t	temp_l;
}ds3231_info;


void DS3231_init(void);							//DS3231初始化
void DS3231_Write(uint8_t Reg_add, uint8_t byte_cnt, uint8_t *buf);	//DS3231写器件
void DS3231_Read(uint8_t Reg_add, uint8_t byte_cnt, uint8_t *buf);	//DS3231读器件
extern inline uint8_t bcd2hex(uint8_t bcd_data);	//BCD转HEX
extern inline uint8_t hex2bcd(uint8_t hex_data);	//HEX转BCD
uint8_t max_date(uint8_t year,uint8_t month);		//计算某年某月的天数
uint8_t week(uint8_t year, uint8_t month, uint8_t date);//根据日期计算星期
int8_t DS3231_Read_temp(void);						//强制启动温度测量并读取

#endif

