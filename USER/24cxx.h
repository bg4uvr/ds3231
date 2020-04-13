/********************************************************************************
	
	STM32F103C8T6ģ�� + DS3231 + 24C32��ģ�� + TM1637�����ģ�� + OLED��ʾģ�� 

	��ʵʱʱ�� + �¶Ȳ��������棬ʵʱOLED��ʾ�¶����ߣ������ʵ����ʾʱ�ӡ�

															�ģ�ʵ����򡭡�

														2019.2.3��� by bg4uvr~

********************************************************************************/



#ifndef __24CXX_H
#define __24CXX_H
#include "myiic.h"   

//Mini STM32������
//24CXX��������(�ʺ�24C01~24C16,24C32~256δ��������!�д���֤!)
//����ԭ��@ALIENTEK
//2010/6/10
//V1.2

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  

#define EE_TYPE 	AT24C32
#define EE_ADD		0xAE
#define PAGE_SIZE	32				//24C32��ҳ��СΪ32�ֽ�

void AT24CXX_Init(void);			//��ʼ��IIC

uint8_t AT24CXX_ByteRead(uint16_t ReadAddr);
void AT24CXX_ByteWrite(uint16_t WriteAddr, uint8_t DataToWrite);
void AT24CXX_Read(uint16_t ReadAddr, uint16_t Len, uint8_t *buf);
void AT24CXX_Write(uint16_t ReadAddr, uint16_t Len, uint8_t *buf);

#endif
















