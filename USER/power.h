#ifndef __POWER_H
#define __POWER_H

#include "stm8s.h"

extern bool halfsec;        //半秒标志
extern bool key_flag;       //已按键标志

void power_init(void);      //初始化电源管理
void power_halt(void);      //休眠程序

#endif