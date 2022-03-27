#ifndef _LED_H_
#define _LED_H_
#include "sys.h"

//LED接口定义
#define LED0 PFout(6)           //PF6
#define LED1 PFout(9)           //PF9
#define LED2 PFout(10)          //PF10
#define LED3 PCout(0)           //PC0


void LED_Init(void);  
#endif
