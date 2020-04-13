/************************************************************
  * @brief   按键驱动
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    button.c
  ***********************************************************/
#include "button.h"
#include "string.h"

//声明链表指针
static struct button* Head_Button = NULL;	//这是一个指针，先声明成空的

//追加按键指针地址到链表中
static void Add_Button(Button_t* btn)
{
	struct button *pass_btn = Head_Button;	//读取链表头到一个静态指针

	while(pass_btn)							//如果指针非空
	{
		pass_btn = pass_btn->Next;			//
	}

	btn->Next = Head_Button;				//按键的下一按键指针，指向上原链表头
	Head_Button = btn;						//把本次按键结构体的地址装入链表头
}

/************************************************************
  * @brief   按键创建
	* @param   name : 按键名称
	* @param   btn : 按键结构体
  * @param   read_btn_level : 按键电平读取函数，需要用户自己实现返回uint8_t类型的电平
  * @param   btn_trigger_level : 按键触发电平
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void Button_Create(const char *name,
                   Button_t *btn,
                   uint8_t(*read_btn_level)(void),
                   uint8_t btn_trigger_level)
{
	memset(btn, 0, sizeof(struct button));  //清除结构体信息，建议用户在之前清除

	btn->Button_State = NONE_TRIGGER;           		//按键状态
	btn->Button_Last_State = NONE_TRIGGER;      		//按键上一次状态
	btn->Button_Trigger_Event = NONE_TRIGGER;   		//按键触发事件
	btn->Read_Button_Level = read_btn_level;    		//按键读电平函数
	btn->Button_Trigger_Level = btn_trigger_level;  	//按键触发电平
	btn->Button_Last_Level = btn->Read_Button_Level();	//按键当前电平
	btn->Debounce_Time = 0;

	Add_Button(btn);          				//创建的时候添加到单链表中
}

/************************************************************
  * @brief   按键触发事件与回调函数映射链接起来
	* @param   btn : 按键结构体
	* @param   btn_event : 按键触发事件
  * @param   btn_callback : 按键触发之后的回调处理函数。需要用户实现
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
void Button_Attach(Button_t *btn,Button_Event btn_event,Button_CallBack btn_callback)
{
	if(BUTTON_ALL_RIGGER == btn_event)						//如果触发了全部事件
	{
		for(uint8_t i = 0 ; i < number_of_event-1 ; i++)
			btn->CallBack_Function[i] = btn_callback;		//按键事件触发的回调函数，用于处理按键事件
	}
	else
	{
		btn->CallBack_Function[btn_event] = btn_callback;	//按键事件触发的回调函数，用于处理按键事件
	}
}

/************************************************************
  * @brief   删除一个已经创建的按键
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void Button_Delete(Button_t *btn)
{
	struct button** curr;
	for(curr = &Head_Button; *curr;)
	{
		struct button* entry = *curr;
		if (entry == btn)
		{
			*curr = entry->Next;
		}
		else
		{
			curr = &entry->Next;
		}
	}
}

/************************************************************
  * @brief   按键周期处理函数
  * @param   btn:处理的按键
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    必须以一定周期调用此函数，建议周期为20~50ms
  ***********************************************************/
void Button_Cycle_Process(Button_t *btn)
{
	uint8_t current_level = (uint8_t)btn->Read_Button_Level();//获取当前按键电平

	//按键电平发生变化，消抖
	if((current_level != btn->Button_Last_Level)&&(++(btn->Debounce_Time) >= BUTTON_DEBOUNCE_TIME))
	{
		btn->Button_Last_Level = current_level; //更新当前按键电平
		btn->Debounce_Time = 0;                 //确定了是按下

		//如果按键是没被按下的，改变按键状态为按下(首次按下/双击按下)
		if((btn->Button_State == NONE_TRIGGER)||(btn->Button_State == BUTTON_DOUBLE))
		{
			btn->Button_State = BUTTON_DOWM;	//按键状态改为按下
		}
		//释放按键
		else if(btn->Button_State == BUTTON_DOWM)//如果状态是已按下
		{
			btn->Button_State = BUTTON_UP;		//状态更改为已释放
			//PRINT_DEBUG("释放了按键");
		}
	}

	//按键状态处理
	switch(btn->Button_State)
	{
	case BUTTON_DOWM :            			//按下状态
	{
		if(btn->Button_Last_Level == btn->Button_Trigger_Level) //如果当前电平和有效状态相同
		{
#if CONTINUOS_TRIGGER    		//支持连续触发

			if(++(btn->Button_Cycle) >= BUTTON_CONTINUOS_CYCLE)
			{
				btn->Button_Cycle = 0;
				btn->Button_Trigger_Event = BUTTON_CONTINUOS;
				TRIGGER_CB(BUTTON_CONTINUOS);    //连按
				//PRINT_DEBUG("连按");
			}

#else

			btn->Button_Trigger_Event = BUTTON_DOWM;	//产生按下事件

			if(++(btn->Long_Time) >= BUTTON_LONG_TIME)  //释放按键前更新触发事件为长按
			{
#if LONG_FREE_TRIGGER

				btn->Button_Trigger_Event = BUTTON_LONG;

#else

			if(++(btn->Button_Cycle) >= BUTTON_LONG_CYCLE)	//连续触发长按的周期
			{
				btn->Button_Cycle = 0;
				btn->Button_Trigger_Event = BUTTON_LONG;	//触发长按事件
				TRIGGER_CB(BUTTON_LONG);					//执行长按事件触发的对象
			}
#endif

				if(btn->Long_Time == 0xFF)  			//更新时间溢出
				{
					btn->Long_Time = BUTTON_LONG_TIME;	//重设为长按最小判断时间
				}
				//PRINT_DEBUG("长按");
			}

#endif
		}

		break;
	}

	case BUTTON_UP :        // 弹起状态
	{
		if(btn->Button_Trigger_Event == BUTTON_DOWM)  //触发单击
		{
			if((btn->Timer_Count <= BUTTON_DOUBLE_TIME)&&(btn->Button_Last_State == BUTTON_DOUBLE)) // 双击
			{
				btn->Button_Trigger_Event = BUTTON_DOUBLE;
				TRIGGER_CB(BUTTON_DOUBLE);
				//PRINT_DEBUG("双击");
				btn->Button_State = NONE_TRIGGER;
				btn->Button_Last_State = NONE_TRIGGER;
			}
			else
			{
				btn->Timer_Count=0;
				btn->Long_Time = 0;   				//检测长按失败，清0

#if (SINGLE_AND_DOUBLE_TRIGGER == 0)
				TRIGGER_CB(BUTTON_DOWM);    		//执行单击事件触发对象
#endif
				btn->Button_State = BUTTON_DOUBLE;
				btn->Button_Last_State = BUTTON_DOUBLE;

			}
		}

		else if(btn->Button_Trigger_Event == BUTTON_LONG)
		{
#if LONG_FREE_TRIGGER
			TRIGGER_CB(BUTTON_LONG);				//长按
#else
			TRIGGER_CB(BUTTON_LONG_FREE);			//长按释放
#endif
			btn->Long_Time = 0;
			btn->Button_State = NONE_TRIGGER;
			btn->Button_Last_State = BUTTON_LONG;
		}

#if CONTINUOS_TRIGGER
		else if(btn->Button_Trigger_Event == BUTTON_CONTINUOS)  //连按
		{
			btn->Long_Time = 0;
			TRIGGER_CB(BUTTON_CONTINUOS_FREE);    	//连发释放
			btn->Button_State = NONE_TRIGGER;
			btn->Button_Last_State = BUTTON_CONTINUOS;
		}
#endif

		break;
	}

	case BUTTON_DOUBLE :
	{
		btn->Timer_Count++;     //时间记录
		if(btn->Timer_Count>=BUTTON_DOUBLE_TIME)
		{
			btn->Button_State = NONE_TRIGGER;
			btn->Button_Last_State = NONE_TRIGGER;
		}
#if SINGLE_AND_DOUBLE_TRIGGER

		if((btn->Timer_Count>=BUTTON_DOUBLE_TIME)&&(btn->Button_Last_State != BUTTON_DOWM))
		{
			btn->Timer_Count=0;
			TRIGGER_CB(BUTTON_DOWM);    //单击
			btn->Button_State = NONE_TRIGGER;
			btn->Button_Last_State = BUTTON_DOWM;
		}

#endif

		break;
	}

	default :
		break;
	}

}

/************************************************************
  * @brief   遍历的方式扫描按键，不会丢失每个按键
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    此函数要周期调用，建议20-50ms调用一次
  ***********************************************************/
void Button_Process(void)
{
	struct button* pass_btn;
	for(pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
	{
		Button_Cycle_Process(pass_btn);
	}
}











