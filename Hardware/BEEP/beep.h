#ifndef _BEEP_H_
#define _BEEP_H_

#include "sys.h"

//BEEP接口定义
#define BEEP PFout(8)           //PF8电平为高，BEEP work    

void BEEP_Init(void);

#endif 
