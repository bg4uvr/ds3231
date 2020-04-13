#include "delay.h"

inline void delay(volatile unsigned char cnt)
{
	while(cnt!=0)   cnt--;
}