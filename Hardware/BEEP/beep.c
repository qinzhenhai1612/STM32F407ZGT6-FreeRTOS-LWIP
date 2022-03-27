#include "beep.h"

void BEEP_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);          //ʹ��GPIOF  clock
	
	//GPIOF8��ʼ������
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOF,GPIO_Pin_8);             //Ĭ��PF8=0,������������  
}