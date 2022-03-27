#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lwip_comm.h"
#include "LAN8720.h"
#include "usmart.h"
#include "lcd.h"
#include "sram.h"
#include "malloc.h"
#include "lwip/netif.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "lwipopts.h"
#include "tcp_client_demo.h"
#include "beep.h"


//ALIENTEK ̽����STM32F407������
//LWIP LWIP�޲���ϵͳ��ֲ����
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾


//��LCD����ʾ��ַ��Ϣ����
//�������ȼ�
#define DISPLAYT_TASK_PRIO		8
//�����ջ��С	
#define DISPLAY_STK_SIZE 		128  
//������
TaskHandle_t DISPLAY_Task_Handler;
//������
void display_task(void *pvParameters);

//LED����
//�������ȼ�
#define LED_TASK_PRIO		9
//�����ջ��С	
#define LED_STK_SIZE 		64  
//������
TaskHandle_t LED_Task_Handler;
//������
void led_task(void *pvParameters);

//��������
//�������ȼ�
#define KEY_TASK_PRIO		10
//�����ջ��С	
#define KEY_STK_SIZE 		128  
//������
TaskHandle_t KEY_Task_Handler;
//������
void key_task(void *pvParameters);


//START����
//�������ȼ�
#define START_TASK_PRIO		11
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t START_Task_Handler;
//������
void start_task(void *pvParameters);


//��LCD����ʾ��ַ��Ϣ
//mode:1 ��ʾDHCP��ȡ���ĵ�ַ
//	  ���� ��ʾ��̬��ַ
void show_address(u8 mode)
{
	u8 buf[30];
	if(mode==2)
	{
		
		sprintf((char*)buf,"DHCP IP :%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//��ӡ��̬IP��ַ
		LCD_ShowString(30,170,210,16,16,buf); 
		sprintf((char*)buf,"DHCP GW :%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//��ӡ���ص�ַ
		LCD_ShowString(30,190,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//��ӡ���������ַ
		LCD_ShowString(30,210,210,16,16,buf); 
		LCD_ShowString(30,230,210,16,16,"Port:8087!"); 

//		sprintf((char*)buf,"MAC    :%d.%d.%d.%d.%d.%d",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);//��ӡMAC��ַ
//		LCD_ShowString(30,130,210,16,16,buf); 
//		sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//��ӡ��̬IP��ַ
//		LCD_ShowString(30,150,210,16,16,buf); 
//		sprintf((char*)buf,"DHCP GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//��ӡ���ص�ַ
//		LCD_ShowString(30,170,210,16,16,buf); 
//		sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//��ӡ���������ַ
//		LCD_ShowString(30,190,210,16,16,buf); 
	}
	else 
	{
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//��ӡ��̬IP��ַ
		LCD_ShowString(30,170,210,16,16,buf); 
		sprintf((char*)buf,"Static GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//��ӡ���ص�ַ
		LCD_ShowString(30,190,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//��ӡ���������ַ
		LCD_ShowString(30,210,210,16,16,buf); 
		LCD_ShowString(30,230,210,16,16,"Port:8087!"); 
		
//		sprintf((char*)buf,"MAC      :%d.%d.%d.%d.%d.%d",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);//��ӡMAC��ַ
//		LCD_ShowString(30,130,210,16,16,buf); 
//		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//��ӡ��̬IP��ַ
//		LCD_ShowString(30,150,210,16,16,buf); 
//		sprintf((char*)buf,"Static GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//��ӡ���ص�ַ
//		LCD_ShowString(30,170,210,16,16,buf); 
//		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//��ӡ���������ַ
//		LCD_ShowString(30,190,210,16,16,buf); 
	}	
}
          
int main(void)
{
	delay_init(168);       	//��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	//����NVIC�жϷ���4
	uart_init(115200);    	//���ڲ���������
	usmart_dev.init(84); 		//��ʼ��USMART
	LED_Init();  						//LED��ʼ��
	KEY_Init();  						//������ʼ��
	LCD_Init(); 						//LCD��ʼ��
	FSMC_SRAM_Init();				//��ʼ���ⲿSRAM  
	
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	my_mem_init(SRAMCCM);	  //��ʼ��CCM�ڴ��
	
	POINT_COLOR = RED; 		
	LCD_ShowString(30,10,200,16,16,"Explorer STM32F4");
	LCD_ShowString(30,30,200,16,16,"Ethernet lwIP Test");
	LCD_ShowString(30,50,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,70,200,20,16,"WKUP:Send data");
	LCD_ShowString(30,90,200,16,16,"2014/8/15"); 	
  
	while(lwip_comm_init()) //lwip��ʼ��
	{
		LCD_ShowString(30,110,200,20,16,"LWIP Init Falied!");   //lwip��ʼ��ʧ��
		delay_ms(1200);
		LCD_Fill(30,110,230,130,WHITE); //�����ʾ
		LCD_ShowString(30,110,200,16,16,"Retrying...");  
	}
	LCD_ShowString(30,110,200,20,16,"LWIP Init Success!");    //lwip��ʼ���ɹ�
	while(tcp_client_init()) 									//��ʼ��tcp_client(����tcp_client�߳�)
	{
		LCD_ShowString(30,150,200,20,16,"TCP Client failed!!"); //tcp�ͻ��˴���ʧ��
		delay_ms(500);
		LCD_Fill(30,150,230,170,WHITE);
		delay_ms(500);
	}
	LCD_ShowString(30,150,200,20,16,"TCP Client Success!"); 	//tcp�ͻ��˴����ɹ�
	
	
	
	//������ʼ����
  xTaskCreate((TaskFunction_t )start_task,            //������
              (const char*    )"start_task",          //��������
              (uint16_t       )START_STK_SIZE,        //�����ջ��С
              (void*          )NULL,                  //���ݸ��������Ĳ���
              (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
              (TaskHandle_t*  )&START_Task_Handler);  //������              
  vTaskStartScheduler();          //�����������
}

//start����
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();      			//�����ٽ���
#if LWIP_DHCP
	lwip_comm_dhcp_creat(); 				//����DHCP����
#endif
	//����LED����
  xTaskCreate((TaskFunction_t )led_task,              //������
              (const char*    )"led_task",            //��������
              (uint16_t       )LED_STK_SIZE,        	//�����ջ��С
              (void*          )NULL,                  //���ݸ��������Ĳ���
              (UBaseType_t    )LED_TASK_PRIO,       	//�������ȼ�
              (TaskHandle_t*  )&LED_Task_Handler);   	//������ 
	
 	//����DISPLAY����
  xTaskCreate((TaskFunction_t )display_task,          //������
              (const char*    )"display_task",        //��������
              (uint16_t       )DISPLAY_STK_SIZE,      //�����ջ��С
              (void*          )NULL,                  //���ݸ��������Ĳ���
              (UBaseType_t    )DISPLAYT_TASK_PRIO,    //�������ȼ�
              (TaskHandle_t*  )&DISPLAY_Task_Handler);//������ 
			  
	//������������
  xTaskCreate((TaskFunction_t )key_task,          //������
              (const char*    )"key_task",        //��������
              (uint16_t       )KEY_STK_SIZE,      //�����ջ��С
              (void*          )NULL,                  //���ݸ��������Ĳ���
              (UBaseType_t    )KEY_TASK_PRIO,    //�������ȼ�
              (TaskHandle_t*  )&KEY_Task_Handler);//������ 		  
			  		  
  vTaskSuspend(START_Task_Handler);//����ʼ����
  taskEXIT_CRITICAL();             //�˳��ٽ���								
}

//��ʾ��ַ����Ϣ
void display_task(void *pvParameters)
{
	while(1)
	{ 

#if LWIP_DHCP									        //������DHCP��ʱ��
		if(lwipdev.dhcpstatus != 0) 			//����DHCP
		{
			show_address(lwipdev.dhcpstatus );	//��ʾ��ַ��Ϣ
			vTaskSuspend(DISPLAY_Task_Handler); //��ʾ���ַ��Ϣ�������������
		}
#else
		show_address(0); 						          //��ʾ��̬��ַ
		vTaskSuspend(DISPLAY_Task_Handler); 	//��ʾ���ַ��Ϣ�������������
#endif //LWIP_DHCP
		vTaskDelay(500);      						//��ʱ500ms
	}
}

//led����
void led_task(void *pvParameters)
{
	while(1)
	{
		LED0 = !LED0;
		vTaskDelay(1000);      //��ʱ500ms
		
 	}
}
void key_task(void *pvParameters)
{
	u8 key;
	while(1)
	{
		key = KEY_Scan(0);
		if(key==WKUP_PRES)  //��������
		{
			tcp_client_flag |= LWIP_SEND_DATA;  //���LWIP������Ҫ����
		}
		vTaskDelay(10);      //��ʱ10ms
	}
}




