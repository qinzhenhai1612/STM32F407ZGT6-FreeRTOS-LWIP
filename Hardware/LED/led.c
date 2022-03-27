#include "led.h"



void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOC,ENABLE);          //使能GPIOF GPIOC clock
	
	
	//GPIOF9,F10初始化设置
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	GPIO_SetBits(GPIOF,GPIO_Pin_6 | GPIO_Pin_9 | GPIO_Pin_10);                 //设置GPIOF6, GPIOF9,F10电平为高时，灯灭
	
	//GPIOC0初始化设置
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_0);                 //设置GPIOC0电平为高时，灯灭
}







