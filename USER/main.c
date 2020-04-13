/*

	《STM8S105K4实验板+DS3231模块+HT1621模式 = 实时时钟实验》

程序较为完整地实现了时间显示、时间设置功能，并充分考虑了节电性能，
使之比较适合使用电池供电来长时间运行，具备一定的实用性。但程序仍然
是实验性的，因为使用了DS3231的SQW脚来输出1Hz信号唤醒MCU，所以程序
没用设计闹钟功能。

注：
1、除STM8官方库文件外，部分基础代码源于实验板例程；按键代码源于网
络；软件IIC驱动基于原子例程修改；DS3231代码、电源管理代码、全部主
程序逻辑为原创。
2、代码中有些源代码文件项目中并未使用，为方便以后使用并未中删除。

bg4uvr	最后版本完成于2020.3.5
*/

#include "stm8s.h"
#include "timer.h"
#include "ht1621.h"
#include "delay.h"
#include "myiic.h"
#include "ds3231.h"
#include "key.h"
#include "button.h"
#include "power.h"

bool last_halfsec;				//半秒信号
uint8_t sysMode;				//系统工作模式
uint8_t timeChanged;			//设置时是否改变了时钟
uint8_t year,month,date,hour,min;	//HEX格式时间（设置时间时使用）
uint8_t buf[4];					//显示缓存
ds3231_info ds3231;             //DS3231结构体

Button_t Button1;               //注册按键1
Button_t Button2;               //注册按键2

//按键1单击
void Btn1_Dowm_CallBack(void *btn)
{
	ModeTimeOutCnt=0;			//模式超时计数器

	switch(sysMode)
	{
		//普通模式，循环切换6种显示状态
	case 0:					//普通模式
	case 1:					//秒
	case 2:					//年
	case 3:					//月。日
	case 4:					//星期
		sysMode++;
		break;
	case 5:					//温度
		sysMode=0;
		break;

		//设置模式
	case 11:				//设置年份的世纪位
		if(year<100)    year+=100;
		else year-=100;
		break;
	case 12:				//设置年份的10年位
		if(year%100 < 90) year+=10;
		else year-=90;
		break;
	case 13:				//设置年份的个位
		if(year%10 < 9) year++;
		else year-=9;
		break;
	case 14:				//设置月的十位
		if(month<10) month=10;
		else month=1;
		break;
	case 15:				//设置月的个位
		month++;
		if(month>=13)    month=10;
		else if(month==10)   month=1;
		break;
	case 16:				//设置日的十位
		date+=10;
		if(date>max_date(year,month))
		{
			if(date/10!=0)
			{
				date=date/10*10;
				if(date>max_date(year,month))
					date=1;

			}
			else
				date-=10;
		}
		break;
	case 17:				//设置日的个位
		date++;
		if(date%10==0)
		{
			date-=10;
			if(date==0) date=1;
		}
		if(date>max_date(year,month))
		{
			date=date/10*10;
		}
		break;
	case 18:				//设置小时10位
		hour+=10;
		if(hour>23)
		{
			if(hour%10!=0)  hour=20;
			else    hour=0;
		}
		timeChanged=1;
		break;
	case 19:				//设置小时个位
		hour++;
		if(hour>23) hour=hour/10*10;
		else if(hour%10==0) hour-=10;
		timeChanged=1;
		break;
	case 20:				//设置分钟10位
		if(min<50) min+=10;
		else    min%=10;
		timeChanged=1;
		break;
	case 21:				//设置分钟个位
		min++;
		if(min>60)  min/=10;
		else if(min%10==0)  min-=10;
		timeChanged=1;
		break;

		//设置完毕，待启动模式
	case 50:
		ds3231.sec=0;
		ds3231.min=hex2bcd(min);
		ds3231.hour=hex2bcd(hour);
		DS3231_Write(0,3,(uint8_t*)&ds3231);    //写入时分秒数据
		sysMode=0;			//回到普通模式
		break;
	}
}


//按键2单击
void Btn2_Dowm_CallBack(void *btn)
{
	ModeTimeOutCnt=0;			//模式超时计数器

	//普通显示模式，如果非主页面，切换回主页面
	if(sysMode > 0 && sysMode < 10)
		sysMode = 0;

	//设置模式，变更为一下项设置
	else if(sysMode >= 10 && sysMode <50)
	{
		if(++sysMode > 21)		//设置模式供有21-11+1种，到第21种返回第11种
			sysMode = 11;
	}
}

//按键2长按
void Btn2_Long_CallBack(void *btn)
{
	ModeTimeOutCnt=0;			//模式超时计数器

	//如果是普通模式，则进入设置模式
	if(sysMode==0)
	{
		sysMode=10;				//进入设置模式
		timeChanged=0;			//时钟已更改状态清零
		//读取DS3231时间数据到缓存
		DS3231_Read(0x00,7,(uint8_t*)&ds3231);
		//把时间转换为hex数值，便于后续设置时计算
		year=ds3231.month&0x80?bcd2hex(ds3231.year)+100:bcd2hex(ds3231.year);
		month=bcd2hex(ds3231.month&0x1f);
		date=bcd2hex(ds3231.date);
		hour=bcd2hex(ds3231.hour);
		min=bcd2hex(ds3231.min);
	}

	//如果是设置模式，则保存设置
	else if(sysMode >=11 && sysMode < 50)
	{
		//把已设置的hex格式数据，转换为bcd格式
		ds3231.date=hex2bcd(date);
		ds3231.month=year/100?hex2bcd(month)|0x80:hex2bcd(month);
		ds3231.year=hex2bcd(year%100);
		//根据日期计算星期
		ds3231.week=week(ds3231.year,ds3231.month,ds3231.date);
		//保存年月日和星期（4个字节）
		DS3231_Write(WEEK,4,(uint8_t*)&ds3231.week);

		//如果更改为时间数据，进入等待启动状态
		if(timeChanged) sysMode=50;
		//否则直接返回普通模式
		else    sysMode=0;
	}
}


//系统初始化
void Sys_init(void)
{
	ht1621Init();				//初始化LCD
	IIC_Init();					//初始化软件IIC
	KEY_Init();					//按键初始化
	DS3231_init();				//DS3231初始化
	power_init();				//休眠模式初始化

	//注册按键1
	Button_Create("Button1",&Button1,Read_KEY1_Level,KEY_ON);
	Button_Attach(&Button1,BUTTON_DOWM,Btn1_Dowm_CallBack);		//单击

	//注册按键2
	Button_Create("Button2",&Button2,Read_KEY2_Level,KEY_ON);
	Button_Attach(&Button2,BUTTON_DOWM,Btn2_Dowm_CallBack);		//单击
	Button_Attach(&Button2,BUTTON_LONG,Btn2_Long_CallBack);		//长按

	Timer4_Init();				//初始化定时器4
}

//主程序
int main(void)
{
	Sys_init();					//初始化
	sysMode=0;					//系统模式初始化

	while(1)
	{
		//检查是否是被按键唤醒
		if(key_flag)
		{
			key_flag=FALSE;		//清空按键标志
			ModeTimeOutCnt=0;	//清零超时计数器
			TIM4_Cmd(ENABLE);	//启动定时器4
		}

		//如果定时器4是开启状态并且已超时
		if(TIM4->CR1&TIM4_CR1_CEN && ModeTimeOutCnt>500)
		{
			sysMode=0;			//恢复普通模式
			TIM4_Cmd(DISABLE);	//关闭定时器4
		}

		//普通模式
		if(sysMode == 0)
		{
			//只读取小时和分钟数据
			DS3231_Read(MIN,2,(uint8_t*)&ds3231.min);
			//显示小时和分钟
			if(ds3231.hour>>4==0)	//小时十位无效0消隐
				buf[0]=halfsec?17:16;
			else
				buf[0]=halfsec?ds3231.hour>>4|0x80:ds3231.hour>>4;
			buf[1]=ds3231.hour&0x0f;
			buf[2]=ds3231.min>>4;
			buf[3]=ds3231.min&0x0f;
			dispAll((uint8_t*)buf);
			//如果已超时，休眠
			if(ModeTimeOutCnt>500) 	power_halt();
		}
		//其他模式
		else
		{
			//每半秒才执行一次
			while(last_halfsec==halfsec)	wfi();	//如果未到半秒，MCU进入Wait模式，等待中断唤醒
			last_halfsec=halfsec;

			//读取全部7个字节的时间数据
			if(sysMode<=10)
			{
				DS3231_Read(0,7,(uint8_t*)&ds3231);
			}

			//根据模式不同，显示相对应的数据
			switch(sysMode)
			{

				//特殊显示模式
				//显示秒数
			case 1:
				buf[0]=17;				//显示冒号
				buf[1]=16;				//空白
				buf[2]=ds3231.sec>>4;	//秒十位
				buf[3]=ds3231.sec&0x0f;	//秒个位
				break;
				//显示年
			case 2:
				buf[0]=2;
				buf[1]=ds3231.month&0x80?1:0;	//月的最高位是世纪
				buf[2]=ds3231.year>>4;
				buf[3]=ds3231.year&0x0f;
				break;
				//显示月、日
			case 3:
				buf[0]=ds3231.month&0x10?1:16;
				buf[1]=ds3231.month&0x0f;
				buf[2]=ds3231.date>>4|0x80;
				buf[3]=ds3231.date&0x0f;
				break;
				//显示星期
			case 4:
				buf[0] = 16;
				buf[1] = 16;
				buf[2] = ds3231.week;
				buf[3] = 16;
				break;
				//显示温度
			case 5:
				DS3231_Read(TEMP_H,2,(uint8_t*)&ds3231.temp_h);	//读取温度
				buf[0] = ds3231.temp_h&0x80?18:16;				//判断是否显示负号
				buf[1] = (ds3231.temp_h&0x7f)/10;
				buf[2] = (ds3231.temp_h&0x7f)%10;
				buf[3] = ds3231.temp_l&0x80?5|0x80:0|0x80;
				break;

				//设置模式（以下模式，数据直接缓存中的，不从DS3231读取）
				//显示 "-SET"
			case 10:
				buf[0] = 18;
				buf[1] = 5;
				buf[2] = 0x0e;
				buf[3] = 7;
				break;
				//设置世纪位
			case 11:
				buf[0] = 2;
				buf[1] = halfsec ? year/100 : 16;
				buf[2] = year%100/10;
				buf[3] = year%10;
				break;
				//设置十年位
			case 12:
				buf[0] = 2;
				buf[1] = year/100?1:0;
				buf[2] = halfsec ? year%100/10 : 16;
				buf[3] = year%10;
				break;
				//设置年个位
			case 13:
				buf[0] = 2;
				buf[1] = year/100?1:0;
				buf[2] = year%100/10;
				buf[3] = halfsec ? year%10 : 16;
				break;
				//设置月十位
			case 14:
				buf[0] = halfsec ? month/10 : 16;
				buf[1] = month%10;
				buf[2] = date/10|0x80;
				buf[3] = date%10;
				break;
				//设置月个位
			case 15:
				buf[0] = month/10?1:16;
				buf[1] = halfsec ? month%10:16;
				buf[2] = date/10|0x80;
				buf[3] = date%10;
				break;
				//设置日十位
			case 16:
				buf[0] = month/10?1:16;
				buf[1] = month%10;
				buf[2] = halfsec ? date/10|0x80 : 16|0x80;
				buf[3] = date%10;
				break;
				//设置日个位
			case 17:
				buf[0] = month/10?1:16;
				buf[1] = month%10;
				buf[2] = date/10|0x80;
				buf[3] = halfsec ? date%10 : 16;
				break;

				//设置小时十位
			case 18:
				buf[0] = halfsec ? hour/10|0x80 : 17;
				buf[1] = hour%10;
				buf[2] = min/10;
				buf[3] = min%10;
				break;
				//设置小时个位
			case 19:
				buf[0] = hour/10|0x80;
				buf[1] = halfsec ? hour%10 : 16;
				buf[2] = min/10;
				buf[3] = min%10;
				break;
				//设置分钟十位
			case 20:
				buf[0] = hour/10|0x80;
				buf[1] = hour%10;
				buf[2] = halfsec ? min/10 : 16;
				buf[3] = min%10;
				break;
				//设置分钟个位
			case 21:
				buf[0] = hour/10|0x80;
				buf[1] = hour%10;
				buf[2] = min/10;
				buf[3] = halfsec ?min%10 : 16;
				break;

				//待启动模式
				//设置时钟后，时钟为停止状态，等待按KEY1键
			case 50:
				buf[0] = hour/10|0x80;
				buf[1] = hour%10;
				buf[2] = min/10;
				buf[3] = min%10;
				ModeTimeOutCnt=0;		//模式超时计数器清零，永不超时
				break;
			}
			dispAll((uint8_t*)buf);			//刷新LCD
		}
	}
}