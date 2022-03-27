实验器材:
	信盈达STM32F4开发板
	
实验目的:
	学习FreeRTOS+LWIP.
	
硬件资源:
	1,LED0(连接在PF6)
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)
	3,信盈达3.5寸TFTLCD模块(通过FSMC驱动,FSMC_NE4接LCD片选/A6接RS)
	4,按键KEY0(PE4)/KEY1(PE3)/KEY2(PE2)/KEY_UP(PA0,也称之为WK_UP)
	5,外部SRAM(IS62WV51216,通过FSMC驱动,FSMC_NE3接SRAM) 

实验现象:
	1.本实验开机后，会初始化屏幕，自动获取LCD驱动IC型号，自动进行DHCP获取IP
	2.LED0用于指示系统程序运行状态。
	3.WKUP_KEY用于通过网络发送字符串。
	4.USMART调试，测试所有封装的接口函数。 
   
注意事项: 
	1,本例程在LCD_Init函数里面(在ILI93xx.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!!  
	2,获取IP的前提是前提是插上网线，否则初始化失败。
	
	