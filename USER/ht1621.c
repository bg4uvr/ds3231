
#include "ht1621.h"
#include "delay.h"

//HT1621指令
#define	BIAS		0x52				//0b1000 0101 0010  1/3duty 4com 1000 010a bXcX
#define	SYSDIS		0X00				//0b1000 0000 0000  关振系统荡器和LCD偏压发生器
#define	SYSEN		0X02				//0b1000 0000 0010	打开系统振荡器
#define	LCDOFF		0X04				//0b1000 0000 0100  关LCD偏压
#define	LCDON		0X06				//0b1000 0000 0110  打开LCD偏压
#define	XTAL		0x28				//0b1000 0010 1000	外部接时钟
#define	RC256		0X30				//0b1000 0011 0000  内部时钟
#define	WDTDIS1		0X0A				//0b1000 0000 1010  禁止看门狗

#define DOT         0x10

#define LCD_TEST


//字形表 0-f，外加空白和全显
const uint8_t dispTab[]=
{
	// 0                                                10   11   12   13   14   15   16   17   18
	// 0,   1,   2,   3,   4,   5,   6    7,   8,   9,   A,   B,   C,   D,   E,   F     ,  .,    -,
	0xEB,0x0A,0xAD,0x8F,0x4E,0xC7,0xE7,0x8A,0xEF,0xCF,0xEE,0x67,0xE1,0x2F,0xE5,0xE4,0x00,0x10,0x04
};


//基础操作定义
void ht1621_CS_0()
{
	GPIO_Init(ht1621_CS_PORT, ht1621_CS_PIN, GPIO_MODE_OUT_PP_LOW_FAST);   //
}
void ht1621_CS_1()
{
	GPIO_Init(ht1621_CS_PORT, ht1621_CS_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
}
void ht1621_WR_0()
{
	GPIO_Init(ht1621_WR_PORT, ht1621_WR_PIN, GPIO_MODE_OUT_PP_LOW_FAST);   //
}
void ht1621_WR_1()
{
	GPIO_Init(ht1621_WR_PORT, ht1621_WR_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
}
void ht1621_DAT_0()
{
	GPIO_Init(ht1621_DAT_PORT, ht1621_DAT_PIN, GPIO_MODE_OUT_PP_LOW_FAST);   //
}
void ht1621_DAT_1()
{
	GPIO_Init(ht1621_DAT_PORT, ht1621_DAT_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
}
void ht1621_GND_0()
{
	GPIO_Init(ht1621_GND_PORT, ht1621_GND_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);   //
}
void ht1621_GND_1()
{
	GPIO_Init(ht1621_GND_PORT, ht1621_GND_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
}
void ht1621_VCC_0()
{
	GPIO_Init(ht1621_VCC_PORT, ht1621_VCC_PIN, GPIO_MODE_OUT_PP_LOW_FAST);   //
}
void ht1621_VCC_1()
{
	GPIO_Init(ht1621_VCC_PORT, ht1621_VCC_PIN, GPIO_MODE_OUT_PP_HIGH_SLOW);
}


//1621写数据函数
void ht1621WrDat(uint8_t dat,uint8_t bitCnt)
{
	uint8_t i;
	for (i=0; i<bitCnt; i++)
	{
		ht1621_WR_0();
		if((dat & 0x80)==0x80)
			ht1621_DAT_1();
		else
			ht1621_DAT_0();
		ht1621_WR_1();
		dat<<=1;
	}
}

//1621写指令函数
void ht1621WrCmd(uint8_t cmd)
{
	ht1621_CS_0();
	ht1621WrDat(0x80,4);		//写入命令标志100
	ht1621WrDat(cmd,8);			//写入命令数据
	ht1621_CS_1();
}

//1621清屏
void ht1621_clear(void)
{
	ht1621_CS_0();
	ht1621WrDat(0xa0,3);		//写入数据标志101
	ht1621WrDat(0x00,6);		//从起始地址

#ifdef LCD_TEST
	ht1621_DAT_1();
	for(uint8_t i=0; i<32; i++)
	{
		ht1621_WR_0();
		delay(1);
		ht1621_WR_1();
		delay(1);
	}
	for(unsigned int i=0; i<500; i++)
		delay(200);
#endif
	ht1621WrDat(0x00,4*32);		//清屏
	ht1621_CS_1();
}

//单独刷新一个字符
void dispByte(uint8_t addr, uint8_t data)
{
	ht1621_CS_0();
	ht1621WrDat(0xa0,3);		//写入数据标志101
	ht1621WrDat(addr*8,6);		//写入地址数据
	ht1621WrDat(data&0x80 ? dispTab[data%sizeof(dispTab)]|DOT : dispTab[data],8);
	ht1621_CS_1();
}

//刷新全屏
void dispAll(uint8_t* dspbuf)
{
	uint8_t i;
	ht1621_CS_0();
	ht1621WrDat(0xa0,3);	//写入数据标志101
	ht1621WrDat(0x00,6);	//从地址0开始写入
	for(i=0; i<4; i++)		//连续写入4个字节的数据
	{
		ht1621WrDat( *dspbuf&0x80 ? dispTab[*dspbuf&0x7f]|DOT : dispTab[*dspbuf],8);
		dspbuf++;
	}
	ht1621_CS_1();
}

//1621初始化函数
void ht1621Init(void)
{
	ht1621_GND_0();			//使用IO脚给HT1621供电
	ht1621_VCC_1();
	delay(100);				//延时一段时间

	ht1621WrCmd(BIAS);
	ht1621WrCmd(RC256);		//使用内部振荡器
	//ht1621WrCmd(XTAL);	//使用外部振荡器
	ht1621WrCmd(SYSDIS);
	ht1621WrCmd(WDTDIS1);
	ht1621WrCmd(SYSEN);
	ht1621WrCmd(LCDON);
	ht1621_clear();			//Clear LCD display
}

