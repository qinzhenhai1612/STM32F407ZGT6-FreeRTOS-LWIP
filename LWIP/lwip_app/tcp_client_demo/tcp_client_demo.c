#include "tcp_client_demo.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "lwip/arch.h"
#include "key.h"
#include "usart.h"
#include "led.h"
#include "beep.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//NETCONN API��̷�ʽ��TCP�ͻ��˲��Դ���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/8/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 

struct netconn *tcp_clientconn;					//TCP CLIENT�������ӽṹ��
u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
u8 *tcp_client_sendbuf="GET / HTTP/1.1\r\nHost: 192.168.1.5:8888\r\nConnection: close\r\n\r\n";	//TCP�ͻ��˷������ݻ�����
//u8 *tcp_client_sendbuf="Explorer STM32F407 NETCONN TCP Client send data\r\n";	//TCP�ͻ��˷������ݻ�����
u8 tcp_client_flag;		//TCP�ͻ������ݷ��ͱ�־λ

////TCP�ͻ�������
//#define TCPCLIENT_PRIO		6
////�����ջ��С
//#define TCPCLIENT_STK_SIZE	300
////�����ջ
//TaskHandle_t TCPCLIENT_TASK_STK[TCPCLIENT_STK_SIZE];

//�������ȼ�
#define TCPCLIENT_PRIO		6
//�����ջ��С	
#define TCPCLIENT_STK_SIZE 	300  
//������
TaskHandle_t TCPCLIENT_Task_Handler;
//������
static void tcp_client_thread(void *pvParameters);






//tcp�ͻ���������
static void tcp_client_thread(void *pvParameters)
{
	
	u32 data_len = 0;
	struct pbuf *q;
	err_t err,recv_err;
	static ip_addr_t server_ipaddr,loca_ipaddr;
	static u16_t 		 server_port,loca_port;
	u8 *CommandStr;
	u8 CommandValue;
	
	LWIP_UNUSED_ARG(0);
	server_port = REMOTE_PORT;
	IP4_ADDR(&server_ipaddr, lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
	
	while (1) 
	{
		tcp_clientconn = netconn_new(NETCONN_TCP);  //����һ��TCP����
		err = netconn_connect(tcp_clientconn,&server_ipaddr,server_port);//���ӷ�����
		if(err != ERR_OK)  netconn_delete(tcp_clientconn); //����ֵ������ERR_OK,ɾ��tcp_clientconn����
		else if (err == ERR_OK)    //���������ӵ�����
		{ 
			struct netbuf *recvbuf;
			tcp_clientconn->recv_timeout = 10;   //��ʱʱ��
			netconn_getaddr(tcp_clientconn,&loca_ipaddr,&loca_port,1); //��ȡ����IP����IP��ַ�Ͷ˿ں�
			printf("�����Ϸ�����%d.%d.%d.%d,�����˿ں�Ϊ:%d\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3],loca_port);
			while(1)
			{
				if((tcp_client_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
				{
					err = netconn_write(tcp_clientconn ,tcp_client_sendbuf,strlen((char*)tcp_client_sendbuf),NETCONN_COPY); //����tcp_server_sentbuf�е�����
					if(err != ERR_OK)
					{
						printf("����ʧ��\r\n");
					}
					tcp_client_flag &= ~LWIP_SEND_DATA;
				}
					
				
				
				
				if((recv_err = netconn_recv(tcp_clientconn,&recvbuf)) == ERR_OK)  //���յ�����
				{	
					taskENTER_CRITICAL(); //���ж�
					memset(tcp_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) memcpy(tcp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//��������
						else memcpy(tcp_client_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_CLIENT_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
					}
					taskEXIT_CRITICAL();  //���ж�
					
				
					CommandStr=tcp_client_recvbuf;
//					LowerToCap(CommandStr,q->len);				//���ַ���ת��Ϊ��д	
//					CommandValue=CommandProcess(CommandStr);
					if(strcmp((char*)CommandStr,"LEDON")==0) LED3=0;
					else if (strcmp((char*)CommandStr,"LEDOFF")==0) LED3=1;
					else if (strcmp((char*)CommandStr,"BEEPON")==0) BEEP=1;
					else if (strcmp((char*)CommandStr,"BEEPOFF")==0) BEEP=0;
					
					printf("%s\r\n",CommandStr);
					memset(CommandStr,0,10);
					data_len=0;  //������ɺ�data_lenҪ���㡣					
					netbuf_delete(recvbuf);
				}else if(recv_err == ERR_CLSD)  //�ر�����
				{
					netconn_close(tcp_clientconn);
					netconn_delete(tcp_clientconn);
					printf("������%d.%d.%d.%d�Ͽ�����\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
					break;
				}
			}
		}
	}
}
               
//����TCP�ͻ����߳�
//����ֵ:0 TCP�ͻ��˴����ɹ�
//		���� TCP�ͻ��˴���ʧ��
u8 tcp_client_init(void)
{
	UBaseType_t res;

	taskENTER_CRITICAL();	//���ж�
	res = xTaskCreate((TaskFunction_t )tcp_client_thread,     //������
              (const char*    )"tcp_client_thread",           //��������
              (uint16_t       )TCPCLIENT_STK_SIZE,            //�����ջ��С
              (void*          )NULL,                 		  //���ݸ��������Ĳ���
              (UBaseType_t    )TCPCLIENT_PRIO,       	      //�������ȼ�
              (TaskHandle_t*  )&TCPCLIENT_Task_Handler);   	  //������ 

//	res = vTaskCreate(tcp_client_thread,(void*)0,(OS_STK*)&TCPCLIENT_TASK_STK[TCPCLIENT_STK_SIZE-1],TCPCLIENT_PRIO); //UCOSII����TCP�ͻ����߳�
	taskEXIT_CRITICAL(); 		//���ж�
	
	return (!res);
}

